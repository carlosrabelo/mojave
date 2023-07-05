#if __has_include(<QApplication>)
#include <cstdio>
#include <memory>
#include <QApplication>
#include <QString>
#include "cpus/z80.hpp"
#include "devices/shared/framebuffer.hpp"
#include "frontend/qt6/mainwindow.hpp"
#include "machines/shared/machine.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Framebuffer fb(256, 192);
    fb.fillSnow();
    auto machine = std::make_unique<Machine>(std::make_unique<Z80>());
    MainWindow window(std::move(machine), fb, QStringLiteral("vsd"));
    window.show();
    return app.exec();
}
#else
#include <cstdio>

int main() {
    std::fprintf(stderr, "Error: Qt6 headers are required to build mojave-qt6.\n");
    return 1;
}
#endif
