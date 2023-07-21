#if __has_include(<QApplication>)
#include <cstdio>
#include <memory>
#include <optional>
#include <QApplication>
#include <QString>
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/screen_registry.hpp"
#include "session/cli.hpp"
#include "session/config.hpp"
#include "frontend/shared/machine_keyboard_bridge.hpp"
#include "frontend/shared/framebuffer_refresh.hpp"
#include "session/loader.hpp"
#include "session/runner.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include <QKeyEvent>
#include "machines/shared/machine.hpp"
#include "frontend/qt6/mainwindow.hpp"

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

} // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    const MojaveConfig config = loadMojaveConfig();
    CLIOptions opts;
    if (argc >= 2) {
        opts = parseCLI(argc, argv, config);
        if (!opts.ok) {
            std::fprintf(stderr, "Error: %s\n", opts.error.c_str());
            return 1;
        }
        if (opts.help) {
            return 0;
        }
    } else {
        opts.machine = "trs80m1l1";
    }

    const BuiltinPresetDescriptor* preset = findBuiltinPreset(opts.machine);
    if (!preset) {
        std::fprintf(stderr, "Error: unknown machine preset '%s'.\n", opts.machine.c_str());
        return 1;
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
    for (const auto& load : opts.loads) {
        if (!loader::loadBinary(load.path.c_str(), machine->bus(), load.address)) {
            std::fprintf(stderr, "Error: failed to load binary '%s' at 0x%04X\n",
                         load.path.c_str(), load.address);
            return 1;
        }
    }

    runner::boot(*machine);
    refreshMachineFramebuffer(*machine, *fb);

    std::optional<MachineKeyboardBridge> keyboard_bridge =
        MachineKeyboardBridge::fromMachine(*machine);

    MainWindow window(std::move(machine), *fb, QString::fromStdString(opts.machine),
                      preset->guest_cpu_clock_hz,
                      keyboard_bridge ? &*keyboard_bridge : nullptr);
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
