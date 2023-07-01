#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/screen_registry.hpp"
#include "frontend/sdl/sdl_screen.hpp"
#include "session/clock.hpp"

namespace {

constexpr uint16_t kPlaceholderFbWidth = 256;
constexpr uint16_t kPlaceholderFbHeight = 192;

void printSdlHelp() {
    std::printf(
        "Usage: mojave-sdl [options]\n"
        "\n"
        "Options:\n"
        "  --vsd              Show the snow-pattern virtual screen demo\n"
        "  --scale <n>        Pixel scale factor (default 2)\n"
        "  -h, --help         Show this help message\n"
    );
}

} // namespace

int main(int argc, char* argv[]) {
    bool show_vsd = false;
    int scale = 2;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--vsd") == 0) {
            show_vsd = true;
        } else if (std::strcmp(argv[i], "--scale") == 0 && i + 1 < argc) {
            scale = std::atoi(argv[++i]);
            if (scale < 1)
                scale = 1;
        } else if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0) {
            printSdlHelp();
            return 0;
        } else {
            printSdlHelp();
            return 0;
        }
    }

    if (!show_vsd) {
        printSdlHelp();
        std::printf("\nUse --vsd for the snow demo.\n");
        return 0;
    }

    Framebuffer fb(kPlaceholderFbWidth, kPlaceholderFbHeight);
    ScreenRegistry registry;
    registry.registerScreen("main", fb);

    SdlScreen screen(fb, scale);
    if (!screen.init()) {
        std::fprintf(stderr, "Failed to initialize SDL.\n");
        return 1;
    }

    std::printf("Virtual screen snow pattern running. Close the window to quit.\n");
    while (screen.pollEvents()) {
        fb.fillSnow();
        screen.render();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(Clock::frameIntervalMs()));
    }
    screen.close();
    return 0;
}
