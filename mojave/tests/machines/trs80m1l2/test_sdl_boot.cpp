#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m1/video_controller.hpp"
#include "session/runner.hpp"

namespace {

Framebuffer* findMachineFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

unsigned countWhitePixels(const Framebuffer& fb) {
    unsigned white = 0;
    for (uint16_t y = 0; y < fb.height(); ++y) {
        for (uint16_t x = 0; x < fb.width(); ++x) {
            if (fb.getPixel(x, y) == 0xFFFFFFFFu)
                ++white;
        }
    }
    return white;
}

} // namespace

TEST_CASE("TRS-80 Model I Level II SDL boot wipe still renders bus VRAM writes", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    runner::boot(*machine);
    fb->fill(0xFF000000u);

    machine->bus().write(Trs80M1L2PresetContract::vram_start, 'A');
    REQUIRE(countWhitePixels(*fb) > 0);
    REQUIRE(fb->getPixel(2, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model I Level II SDL boot wipe still renders CPU VRAM writes", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    runner::boot(*machine);
    fb->fill(0xFF000000u);

    // LD A,'A'; LD (0x3C00),A; HALT
    machine->bus().write(0x4000, 0x3E);
    machine->bus().write(0x4001, 0x41);
    machine->bus().write(0x4002, 0x32);
    machine->bus().write(0x4003, 0x00);
    machine->bus().write(0x4004, 0x3C);
    machine->bus().write(0x4005, 0x76);
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x4000;

    machine->step();
    machine->step();
    machine->step();
    machine->step();
    machine->step();
    machine->step();

    REQUIRE(countWhitePixels(*fb) > 0);
    REQUIRE(fb->getPixel(2, 2) == 0xFFFFFFFFu);
}
