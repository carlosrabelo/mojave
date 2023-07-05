#ifndef MOJAVE_MAINWINDOW_HPP
#define MOJAVE_MAINWINDOW_HPP

#include <memory>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTimer>
#include <QToolBar>
#include <QAction>
#include <QStyle>
#include <QLabel>
#include <QWidget>
#include <QSizePolicy>
#include <QString>
#include <QMenuBar>
#include <QMenu>
#include <QEvent>
#include <QKeyEvent>
#include <optional>
#include "machines/shared/machine.hpp"
#include "session/clock.hpp"
#include "session/runner.hpp"
#include "frontend/qt6/virtual_screen_widget.hpp"
#include "frontend/shared/machine_keyboard_bridge.hpp"
#include "frontend/shared/framebuffer_refresh.hpp"

class MainWindow : public QMainWindow {
public:
    enum class RunState { Stopped, Paused, Running };

    explicit MainWindow(std::unique_ptr<Machine> machine, Framebuffer& fb,
                        const QString& machine_name = QString(),
                        uint64_t guest_cpu_clock_hz = 0,
                        MachineKeyboardBridge* keyboard_bridge = nullptr,
                        QWidget* parent = nullptr)
        : QMainWindow(parent),
          machine_(std::move(machine)),
          fb_(fb),
          machine_name_(machine_name),
          clock_(guest_cpu_clock_hz),
          keyboard_bridge_(keyboard_bridge) {
        setWindowTitle(machine_name_.isEmpty()
                           ? QStringLiteral("mojave-qt6")
                           : QStringLiteral("mojave-qt6 - %1").arg(machine_name_));
        resize(800, 600);
        setFocusPolicy(Qt::StrongFocus);

        mdi_area_ = new QMdiArea(this);
        setCentralWidget(mdi_area_);

        screen_widget_ = new VirtualScreenWidget(fb_, this);
        monitor_window_ = mdi_area_->addSubWindow(screen_widget_);
        monitor_window_->setWindowTitle("Monitor");
        monitor_window_->resize(fb_.width() * 2, fb_.height() * 2);
        monitor_window_->setAttribute(Qt::WA_DeleteOnClose, false);
        monitor_window_->installEventFilter(this);
        screen_widget_->show();

        frame_timer_ = new QTimer(this);
        frame_timer_->setInterval(Clock::frameIntervalMs());
        connect(frame_timer_, &QTimer::timeout, this, [this]() { tick(); });

        buildMenu();
        buildControlPanel();
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (keyboard_bridge_ && keyboard_bridge_->handleQtKeyEvent(event, machine_.get())) {
            event->accept();
            return;
        }
        QMainWindow::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        if (keyboard_bridge_ && keyboard_bridge_->handleQtKeyEvent(event, machine_.get())) {
            event->accept();
            return;
        }
        QMainWindow::keyReleaseEvent(event);
    }

    bool eventFilter(QObject* obj, QEvent* ev) override {
        if (obj == monitor_window_ && monitor_action_) {
            if (ev->type() == QEvent::Close) {
                monitor_action_->setChecked(false);
            } else if (ev->type() == QEvent::Show) {
                monitor_action_->setChecked(true);
            }
        }
        return QMainWindow::eventFilter(obj, ev);
    }

private:
    void buildMenu() {
        QMenu* view_menu = menuBar()->addMenu("View");
        monitor_action_ = view_menu->addAction("Monitor");
        monitor_action_->setCheckable(true);
        monitor_action_->setChecked(true);
        connect(monitor_action_, &QAction::toggled, this, [this](bool on) {
            if (!monitor_window_) return;
            if (on) {
                monitor_window_->show();
                screen_widget_->show();
                screen_widget_->update();
                mdi_area_->setActiveSubWindow(monitor_window_);
            } else {
                monitor_window_->hide();
            }
        });
    }

    void buildControlPanel() {
        QToolBar* toolbar = addToolBar("Controls");
        toolbar->setMovable(false);

        play_action_ = toolbar->addAction(
            style()->standardIcon(QStyle::SP_MediaPlay), "Play");
        pause_action_ = toolbar->addAction(
            style()->standardIcon(QStyle::SP_MediaPause), "Pause");
        stop_action_ = toolbar->addAction(
            style()->standardIcon(QStyle::SP_MediaStop), "Stop");

        connect(play_action_, &QAction::triggered, this, [this]() { play(); });
        connect(pause_action_, &QAction::triggered, this, [this]() { pause(); });
        connect(stop_action_, &QAction::triggered, this, [this]() { stop(); });

        toolbar->addSeparator();
        QWidget* spacer = new QWidget(toolbar);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        toolbar->addWidget(spacer);
        state_label_ = new QLabel(toolbar);
        toolbar->addWidget(state_label_);

        updateControls();
    }

    void play() {
        run_state_ = RunState::Running;
        frame_timer_->start();
        updateControls();
    }

    void pause() {
        if (run_state_ != RunState::Running) return;
        run_state_ = RunState::Paused;
        frame_timer_->stop();
        updateControls();
    }

    void stop() {
        run_state_ = RunState::Stopped;
        frame_timer_->stop();
        if (machine_) machine_->reset();
        clock_.reset();
        fb_.fill(0xFF000000u);
        screen_widget_->update();
        updateControls();
    }

    void tick() {
        if (run_state_ != RunState::Running) return;
        if (machine_)
            runner::stepForFrame(*machine_, clock_);
        if (keyboard_bridge_)
            keyboard_bridge_->tick();
        if (machine_)
            refreshMachineFramebuffer(*machine_, fb_);
        screen_widget_->update();
    }

    void updateControls() {
        const bool running = run_state_ == RunState::Running;
        const bool paused = run_state_ == RunState::Paused;
        const bool stopped = run_state_ == RunState::Stopped;
        play_action_->setEnabled(stopped || paused);
        pause_action_->setEnabled(running);
        stop_action_->setEnabled(running || paused);

        const char* text = stopped ? "Stopped" : (paused ? "Paused" : "Running");
        state_label_->setText(QStringLiteral("State: %1  ").arg(text));
    }

    std::unique_ptr<Machine> machine_;
    Framebuffer& fb_;
    QString machine_name_;
    Clock clock_;
    MachineKeyboardBridge* keyboard_bridge_ = nullptr;
    QMdiArea* mdi_area_ = nullptr;
    QMdiSubWindow* monitor_window_ = nullptr;
    VirtualScreenWidget* screen_widget_ = nullptr;
    QTimer* frame_timer_ = nullptr;
    QAction* play_action_ = nullptr;
    QAction* pause_action_ = nullptr;
    QAction* stop_action_ = nullptr;
    QAction* monitor_action_ = nullptr;
    QLabel* state_label_ = nullptr;
    RunState run_state_ = RunState::Stopped;
};

#endif
