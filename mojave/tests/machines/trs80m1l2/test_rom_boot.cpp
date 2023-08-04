#include <cstdio>
#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l1/trs80m1l1_preset.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "session/loader.hpp"
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

unsigned countPrintableVramLetters(Machine& machine) {
    unsigned letters = 0;
    for (uint16_t offset = 0; offset < 1024; ++offset) {
        const uint8_t ch = machine.bus().read(
            static_cast<uint16_t>(Trs80M1L2PresetContract::vram_start + offset));
        // Level II BASIC stores letters using the Model I character encoding
        // (A-Z as 0x01-0x1A) as well as plain ASCII (0x41-0x5A).
        const bool is_ascii_letter = (ch >= 'A' && ch <= 'Z');
        const bool is_model1_letter = (ch >= 0x01 && ch <= 0x1A);
        if (is_ascii_letter || is_model1_letter)
            ++letters;
    }
    return letters;
}

const char* kLevel2RomPath = "/home/carlos/Sources/my-roms/trs80/model1-level2.bin";

} // namespace

TEST_CASE("TRS-80 Model I Level I ROM boot produces visible screen text",
          "[machine][trs80m1l1][integration][rom]") {
    const char* path = "/home/carlos/Sources/my-roms/trs80/model1-level1.bin";
    if (std::fopen(path, "rb") == nullptr)
        return;

    auto machine = createTrs80M1L1Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    REQUIRE(loader::loadBinary(path, machine->bus(), 0x0000));

    runner::boot(*machine);
    fb->fill(0xFF000000u);

    Clock clock(Trs80M1L1PresetContract::guest_cpu_clock_hz);
    for (int frame = 0; frame < 600 && !machine->cpu().halted(); ++frame)
        runner::stepForFrame(*machine, clock);

    const unsigned white = countWhitePixels(*fb);
    const auto& regs = dynamic_cast<Z80&>(machine->cpu()).regs();
    INFO("PC=0x" << std::hex << regs.pc << " white=" << std::dec << white);
    REQUIRE(white > 0);
}

TEST_CASE("TRS-80 Model I Level II ROM boot produces visible screen text",
          "[machine][trs80m1l2][integration][rom]") {
    if (std::fopen(kLevel2RomPath, "rb") == nullptr)
        return;

    auto machine = createTrs80M1L2Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    REQUIRE(loader::loadBinary(kLevel2RomPath, machine->bus(),
                               Trs80M1L2PresetContract::load_rom_address));

    runner::boot(*machine);
    fb->fill(0xFF000000u);

    Clock clock(Trs80M1L2PresetContract::guest_cpu_clock_hz);
    for (int frame = 0; frame < 6000 && !machine->cpu().halted(); ++frame)
        runner::stepForFrame(*machine, clock);

    const unsigned white = countWhitePixels(*fb);
    const unsigned letters = countPrintableVramLetters(*machine);
    const auto& regs = dynamic_cast<Z80&>(machine->cpu()).regs();

    uint8_t vram0 = machine->bus().read(Trs80M1L2PresetContract::vram_start);
    INFO("PC=0x" << std::hex << regs.pc << " SP=0x" << regs.sp << " white=" << std::dec << white
                 << " vram_letters=" << letters << " vram0=0x" << std::hex << static_cast<unsigned>(vram0));
    REQUIRE(letters > 0);
    REQUIRE(white > 0);
}
