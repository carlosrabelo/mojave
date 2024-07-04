#include <cstdint>
#include "catch.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/zx80/display_file.hpp"

using Contract = Zx80PresetContract;

TEST_CASE("Sinclair ZX-80 preset contract memory map", "[machine][zx80][fast]") {
    REQUIRE(Contract::ram_bytes == 1024);
    REQUIRE(Contract::guest_cpu_clock_hz == 3'250'000);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::rom_end_exclusive == 0x1000);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 0x4400);

    REQUIRE(Contract::sysvar_start == 0x4000);
    REQUIRE(Contract::err_nr_address == 0x4000);
    REQUIRE(Contract::d_file_ptr_address == 0x400C);
    REQUIRE(Contract::dfile_halt_opcode == Zx80DisplayFile::kHaltOpcode);
    REQUIRE(Contract::dfile_columns == Zx80DisplayFile::kColumns);
    REQUIRE(Contract::dfile_rows == Zx80DisplayFile::kRows);
    REQUIRE(Contract::dfile_min_collapsed_bytes == Zx80DisplayFile::kMinCollapsedBytes);
    REQUIRE(Contract::dfile_max_bytes == Zx80DisplayFile::kMaxBytes);
    REQUIRE(Contract::charset_rom_start == 0x0E00);
    REQUIRE(Contract::charset_rom_end_exclusive == 0x1000);
    REQUIRE(Contract::charset_i_register == 0x0E);
    REQUIRE(Contract::vram_mirror_start == 0xC000);
    REQUIRE(Contract::vram_mirror_end_exclusive == 65536u);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x1000);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 0x4400);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("Sinclair ZX-80 preset id", "[machine][zx80][fast]") {
    REQUIRE(isZx80PresetId("zx80"));
    REQUIRE_FALSE(isZx80PresetId("zxspectrum"));
    REQUIRE_FALSE(isZx80PresetId("z80"));
}

TEST_CASE("Sinclair ZX-80 machine maps 4 KiB ROM and 1 KiB RAM", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    machine->bus().write(0x0FFF, 0xBB);
    REQUIRE(machine->bus().read(0x0FFF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x43FF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x43FF) == 0x22);
}

TEST_CASE("Sinclair ZX-80 unmapped addresses read floating bus", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    REQUIRE(machine->bus().read(0x1000) == 0xFF);
    REQUIRE(machine->bus().read(0x3FFF) == 0xFF);
    REQUIRE(machine->bus().read(0x4400) == 0xFF);
    REQUIRE(machine->bus().read(0xBFFF) == 0xFF);
    REQUIRE(machine->bus().read(0xC000) == 0xFF);
}

TEST_CASE("Sinclair ZX-80 tiny program runs from RAM", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

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
