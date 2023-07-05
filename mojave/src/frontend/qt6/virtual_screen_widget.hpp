#ifndef MOJAVE_VIRTUAL_SCREEN_WIDGET_HPP
#define MOJAVE_VIRTUAL_SCREEN_WIDGET_HPP

#include <QWidget>
#include <QPainter>
#include <QImage>
#include "devices/shared/framebuffer.hpp"

class VirtualScreenWidget : public QWidget {
public:
    explicit VirtualScreenWidget(Framebuffer& fb, QWidget* parent = nullptr)
        : QWidget(parent), fb_(fb) {
        setMinimumSize(fb_.width(), fb_.height());
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter painter(this);

        painter.fillRect(rect(), Qt::black);

        QImage img(
            reinterpret_cast<const uchar*>(fb_.pixels()),
            fb_.width(),
            fb_.height(),
            fb_.width() * 4,
            QImage::Format_RGBX8888
        );

        QSize scaled = img.size().scaled(size(), Qt::KeepAspectRatio);
        QRect target(QPoint((width() - scaled.width()) / 2,
                            (height() - scaled.height()) / 2),
                     scaled);
        painter.drawImage(target, img);
    }

private:
    Framebuffer& fb_;
};

#endif
