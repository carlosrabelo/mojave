#include <cstdint>
#include "catch.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/zx81/display_file.hpp"
#include "devices/zx81/character_rom.hpp"
#include "devices/zx81/video_generator.hpp"

using Contract = Zx81PresetContract;

TEST_CASE("Sinclair ZX-81 preset contract memory map", "[machine][zx81][fast]") {
    REQUIRE(Contract::ram_bytes == 1024);
    REQUIRE(Contract::guest_cpu_clock_hz == 3'250'000);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::rom_end_exclusive == 0x2000);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 0x4400);

    REQUIRE(Contract::sysvar_start == 0x4000);
    REQUIRE(Contract::sysvar_end_exclusive == 0x407D);
    REQUIRE(Contract::err_nr_address == 0x4000);
    REQUIRE(Contract::d_file_ptr_address == 0x400C);
    REQUIRE(Contract::cdflag_address == 0x403B);
    REQUIRE(Contract::dfile_newline == Zx81DisplayFile::kNewline);
    REQUIRE(Contract::dfile_columns == Zx81DisplayFile::kColumns);
    REQUIRE(Contract::dfile_rows == Zx81DisplayFile::kRows);
    REQUIRE(Contract::dfile_min_collapsed_bytes == Zx81DisplayFile::kMinCollapsedBytes);
    REQUIRE(Contract::dfile_max_bytes == Zx81DisplayFile::kMaxBytes);
    REQUIRE(Contract::charset_rom_start == Zx81CharacterRom::kCharsetBase);
    REQUIRE(Contract::charset_rom_end_exclusive == Zx81CharacterRom::kCharsetEndExclusive);
    REQUIRE(Contract::charset_i_register == Zx81CharacterRom::kIRegister);
    REQUIRE(Contract::video_frame_hz == Zx81VideoGenerator::kFrameHz);
    REQUIRE(Contract::video_framebuffer_width == Zx81VideoGenerator::kFramebufferWidth);
    REQUIRE(Contract::video_framebuffer_height == Zx81VideoGenerator::kFramebufferHeight);
    REQUIRE(Contract::cdflag_slow_display_bit == Zx81VideoGenerator::kCdflagSlowDisplayBit);
    REQUIRE(Contract::cdflag_slow_display_mask == Zx81VideoGenerator::kCdflagSlowDisplayMask);
    REQUIRE(Contract::nmi_hz == 50u);
    REQUIRE(Contract::nmi_cycles_per_interrupt == 65'000u);
    REQUIRE(Contract::keyboard_port_low_byte == 0x00FE);
    REQUIRE(Contract::keyboard_row_count == 8);
    REQUIRE(Contract::keyboard_bits_per_row == 5);
    REQUIRE(Contract::io_port_attach_start == 0x0000);
    REQUIRE(Contract::io_port_attach_end_exclusive == 0x0000);
    REQUIRE(Contract::cassette_baud == 250);
    REQUIRE(Contract::cassette_ear_bit == 0x80);
    REQUIRE(Contract::vram_mirror_start == 0xC000);
    REQUIRE(Contract::vram_mirror_end_exclusive == 65536u);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x2000);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 0x4400);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("Sinclair ZX-81 preset id", "[machine][zx81][fast]") {
    REQUIRE(isZx81PresetId("zx81"));
    REQUIRE_FALSE(isZx81PresetId("zx80"));
    REQUIRE_FALSE(isZx81PresetId("zxspectrum"));
    REQUIRE_FALSE(isZx81PresetId("z80"));
}

TEST_CASE("Sinclair ZX-81 machine maps 8 KiB ROM and 1 KiB RAM", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    machine->bus().write(0x1FFF, 0xBB);
    REQUIRE(machine->bus().read(0x1FFF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x43FF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x43FF) == 0x22);
}

TEST_CASE("Sinclair ZX-81 unmapped addresses read floating bus", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    REQUIRE(machine->bus().read(0x2000) == 0xFF);
    REQUIRE(machine->bus().read(0x3FFF) == 0xFF);
    REQUIRE(machine->bus().read(0x4400) == 0xFF);
    REQUIRE(machine->bus().read(0xBFFF) == 0xFF);
    REQUIRE(machine->bus().read(0xC000) == 0xFF);
}

TEST_CASE("Sinclair ZX-81 tiny program runs from RAM", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    machine->bus().write(0x4000, 0x00); // NOP
    machine->bus().write(0x4001, 0x76); // HALT
    machine->reset();
    auto* z80 = dynamic_cast<Z80*>(&machine->cpu());
    REQUIRE(z80 != nullptr);
    z80->regs().pc = 0x4000;

    REQUIRE_FALSE(machine->cpu().halted());
    REQUIRE(machine->step() == 4);
    REQUIRE_FALSE(machine->cpu().halted());
    REQUIRE(machine->step() == 4);
    REQUIRE(machine->cpu().halted());
}
