#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l1/trs80m1l1_preset.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/screen_registry.hpp"
#include "devices/trs80m1/video_controller.hpp"
#include "session/runner.hpp"
#include "session/clock.hpp"
#include "cpus/z80.hpp"

namespace {

Framebuffer* findMachineFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

} // namespace

TEST_CASE("TRS-80 Model I Level I preset is eligible for SDL and Qt6 frontends",
          "[machine][trs80m1l1][integration]") {
    const BuiltinPresetDescriptor* preset = findBuiltinPreset("trs80m1l1");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->needs_virtual_screen);

    auto machine = createBuiltinMachine("trs80m1l1");
    REQUIRE(machine != nullptr);

    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);
    REQUIRE(fb->width() == Trs80M1VideoController::kFramebufferWidth);
    REQUIRE(fb->height() == Trs80M1VideoController::kFramebufferHeight);
}

TEST_CASE("TRS-80 Model I Level I virtual screen shows VRAM through the screen registry",
          "[machine][trs80m1l1][integration]") {
    auto machine = createTrs80M1L1Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    ScreenRegistry registry;
    registry.registerScreen("main", *fb);

    runner::boot(*machine);
    REQUIRE(fb->getPixel(0, 0) == 0xFF000000u);

    machine->bus().write(Trs80M1L1PresetContract::vram_start, 'A');
    machine->bus().write(Trs80M1L1PresetContract::vram_start + 1, 'B');

    Framebuffer* screen = registry.screen("main");
    REQUIRE(screen == fb);
    REQUIRE(screen->getPixel(2, 2) == 0xFFFFFFFFu);
    REQUIRE(screen->getPixel(8, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model I Level I frontend frame loop uses guest clock pacing",
          "[machine][trs80m1l1][integration]") {
    auto machine = createTrs80M1L1Machine();
    const BuiltinPresetDescriptor* preset = findBuiltinPreset("trs80m1l1");
    REQUIRE(preset != nullptr);

    Clock clock(preset->guest_cpu_clock_hz);
    runner::boot(*machine);

    machine->bus().write(Trs80M1L1PresetContract::ram_start, 0x18);
    machine->bus().write(Trs80M1L1PresetContract::ram_start + 1, 0xFE);
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = Trs80M1L1PresetContract::ram_start;

    const unsigned cycles = runner::stepForFrame(*machine, clock);
    REQUIRE(cycles == clock.cyclesPerFrame());
}
