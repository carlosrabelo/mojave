#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"

using Contract = Trs80M1L2PresetContract;

TEST_CASE("TRS-80 Model I Level II preset contract memory map", "[machine][trs80m1l2][fast]") {
    REQUIRE(Contract::base_ram_bytes == 16384);
    REQUIRE(Contract::expansion_ram_bytes == 32768);
    REQUIRE(Contract::ram_bytes == 49152);
    REQUIRE(Contract::guest_cpu_clock_hz == 1'774'080);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::rom_end_exclusive == 0x3000);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 0x8000);
    REQUIRE(Contract::expansion_ram_start == 0x8000);
    REQUIRE(Contract::expansion_ram_end_exclusive == 65536u);

    REQUIRE(Contract::printer_status_address == 0x37E8);
    REQUIRE(Contract::keyboard_start == 0x3800);
    REQUIRE(Contract::keyboard_end_exclusive == 0x3C00);
    REQUIRE(Contract::vram_start == 0x3C00);
    REQUIRE(Contract::vram_end_exclusive == 0x4000);
    REQUIRE(Contract::expansion_port_start == 0xE8);
    REQUIRE(Contract::expansion_port_end_exclusive == 0xF0);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x3000);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 65536u);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("TRS-80 Model I Level II preset id", "[machine][trs80m1l2][fast]") {
    REQUIRE(isTrs80M1L2PresetId("trs80m1l2"));
    REQUIRE_FALSE(isTrs80M1L2PresetId("trs80m1l1"));
    REQUIRE_FALSE(isTrs80M1L2PresetId("z80"));
}

TEST_CASE("TRS-80 Model I Level II machine maps 12 KiB ROM and 16 KiB base RAM", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    REQUIRE(machine->bus().read(0x2FFF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x7FFF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x7FFF) == 0x22);
}

TEST_CASE("TRS-80 Model I Level II unmapped regions read as floating bus 0xFF", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    REQUIRE(machine->bus().read(0x3000) == 0xFF);
    REQUIRE(machine->bus().read(0x37E0) == 0xFF);
    REQUIRE(machine->bus().read(0x8000) == 0xFF);
    REQUIRE(machine->bus().read(0xFFFF) == 0xFF);

    machine->bus().write(0x3500, 0x55);
    REQUIRE(machine->bus().read(0x3500) == 0xFF);
}
