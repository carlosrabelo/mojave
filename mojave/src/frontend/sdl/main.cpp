#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <optional>
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/screen_registry.hpp"
#include "frontend/sdl/sdl_screen.hpp"
#include "session/cli.hpp"
#include "session/config.hpp"
#include "session/clock.hpp"
#include "session/loader.hpp"
#include "session/runner.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include "frontend/shared/machine_keyboard_bridge.hpp"
#include "frontend/shared/framebuffer_refresh.hpp"

namespace {

constexpr uint16_t kPlaceholderFbWidth = 256;
constexpr uint16_t kPlaceholderFbHeight = 192;

Framebuffer* findMachineFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

void printSdlHelp() {
    std::printf(
        "Usage: mojave-sdl [options]\n"
        "\n"
        "Options:\n"
        "  --machine <name>   Machine preset (z80, m6502, m6507, trs80m1l1, trs80m1l2, trs80m3)\n"
        "                     Presets without a display use a blank placeholder window\n"
        "  --load-bin <path> [<addr>]\n"
        "                     Load binary at hex address or rom/ram alias\n"
        "  --vsd              Show the snow-pattern virtual screen demo\n"
        "  --scale <n>        Pixel scale factor (default 2)\n"
        "  -h, --help         Show this help message\n"
    );
}

} // namespace

int main(int argc, char* argv[]) {
    bool show_vsd = false;
    int scale = 2;
    std::vector<char*> cli_argv;
    cli_argv.push_back(argv[0]);

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--vsd") == 0) {
            show_vsd = true;
        } else if (std::strcmp(argv[i], "--scale") == 0 && i + 1 < argc) {
            scale = std::atoi(argv[++i]);
            if (scale < 1)
                scale = 1;
        } else {
            cli_argv.push_back(argv[i]);
        }
    }

    const MojaveConfig config = loadMojaveConfig();
    CLIOptions opts;
    bool parsed_cli = cli_argv.size() > 1;
    if (parsed_cli) {
        opts = parseCLI(static_cast<int>(cli_argv.size()), cli_argv.data(), config);
        if (!opts.ok) {
            std::fprintf(stderr, "Error: %s\n", opts.error.c_str());
            return 1;
        }
        if (opts.help) {
            printSdlHelp();
            return 0;
        }
    }

    const BuiltinPresetDescriptor* preset =
        parsed_cli ? findBuiltinPreset(opts.machine) : nullptr;
    const bool run_machine = preset != nullptr && !show_vsd;

    if (!run_machine) {
        if (!show_vsd) {
            printSdlHelp();
            std::printf("\nUse --vsd for the snow demo or --machine <preset> to run a machine.\n");
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

    std::unique_ptr<Machine> machine = createBuiltinMachine(opts.machine);
    if (!machine) {
        std::fprintf(stderr, "Error: failed to create machine preset '%s'.\n", opts.machine.c_str());
        return 1;
    }

    std::unique_ptr<Framebuffer> placeholder_fb;
    Framebuffer* fb = findMachineFramebuffer(*machine);
    if (!fb) {
        placeholder_fb = std::make_unique<Framebuffer>(kPlaceholderFbWidth, kPlaceholderFbHeight);
        fb = placeholder_fb.get();
        fb->fill(0xFF000000u);
        std::printf("Machine preset '%s' has no display; using a blank placeholder window.\n",
                    opts.machine.c_str());
    }

    ScreenRegistry registry;
    registry.registerScreen("main", *fb);

    SdlScreen screen(*fb, scale);
    if (!screen.init(true)) {
        std::fprintf(stderr, "Failed to initialize SDL.\n");
        return 1;
    }

    for (const auto& load : opts.loads) {
        if (!loader::loadBinary(load.path.c_str(), machine->bus(), load.address)) {
            std::fprintf(stderr, "Error: failed to load binary file '%s' at address 0x%04X\n",
                         load.path.c_str(), load.address);
            return 1;
        }
    }

    runner::boot(*machine);
    refreshMachineFramebuffer(*machine, *fb);

    std::optional<MachineKeyboardBridge> keyboard_bridge =
        MachineKeyboardBridge::fromMachine(*machine);

    std::printf("Starting SDL screen for machine preset '%s'. Close the window to quit.\n",
                opts.machine.c_str());

    Clock clock(preset->guest_cpu_clock_hz);
    while (screen.pollEvents(keyboard_bridge ? &*keyboard_bridge : nullptr, machine.get())) {
        runner::stepAndPaceFrame(*machine, clock);
        if (keyboard_bridge)
            keyboard_bridge->tick();
        refreshMachineFramebuffer(*machine, *fb);
        screen.render();
    }

    screen.close();
    return 0;
}
