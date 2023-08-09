#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"

using Contract = Trs80M3PresetContract;

TEST_CASE("TRS-80 Model III preset contract memory map", "[machine][trs80m3][fast]") {
    REQUIRE(Contract::ram_bytes == 49152);
    REQUIRE(Contract::guest_cpu_clock_hz == 2'027'520);
    REQUIRE(Contract::rtc_interrupt_hz == 30);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::io_latch_start == 0x37E0);
    REQUIRE(Contract::io_latch_end_exclusive == 0x37F0);
    REQUIRE(Contract::rom_tail_start == 0x37F0);
    REQUIRE(Contract::rom_end_exclusive == 0x3800);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 65536u);

    REQUIRE(Contract::printer_status_address == 0x37E8);
    REQUIRE(Contract::keyboard_start == 0x3800);
    REQUIRE(Contract::keyboard_end_exclusive == 0x3C00);
    REQUIRE(Contract::vram_start == 0x3C00);
    REQUIRE(Contract::vram_end_exclusive == 0x4000);
    REQUIRE(Contract::port_decode_start == 0xE0);
    REQUIRE(Contract::port_decode_end_exclusive == 0xF0);
    REQUIRE(Contract::interrupt_latch_port_start == 0xE0);
    REQUIRE(Contract::interrupt_latch_port_end_exclusive == 0xE4);
    REQUIRE(Contract::hardware_control_port_start == 0xEC);
    REQUIRE(Contract::hardware_control_port_end_exclusive == 0xF0);
    REQUIRE(Contract::cassette_screen_port_start == 0xFC);
    REQUIRE(Contract::cassette_screen_port_end_exclusive == 0x100);
    REQUIRE(Contract::floppy_port_start == 0xF0);
    REQUIRE(Contract::floppy_port_end_exclusive == 0xF5);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x3800);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 65536u);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("TRS-80 Model III preset id", "[machine][trs80m3][fast]") {
    REQUIRE(isTrs80M3PresetId("trs80m3"));
    REQUIRE_FALSE(isTrs80M3PresetId("trs80m1l2"));
    REQUIRE_FALSE(isTrs80M3PresetId("z80"));
}

TEST_CASE("TRS-80 Model III machine maps 14 KiB ROM and 48 KiB RAM", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    REQUIRE(machine->bus().read(0x37DF) == 0x00);
    REQUIRE(machine->bus().read(0x37F0) == 0x00);
    REQUIRE(machine->bus().read(0x37FF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x7FFF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x7FFF) == 0x22);

    machine->bus().write(0x8000, 0x33);
    machine->bus().write(0xFFFF, 0x44);
    REQUIRE(machine->bus().read(0x8000) == 0x33);
    REQUIRE(machine->bus().read(0xFFFF) == 0x44);
}

TEST_CASE("TRS-80 Model III I/O latch window is reserved between ROM spans",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    REQUIRE(machine->bus().read(Contract::io_latch_start) == 0xFF);
    REQUIRE(machine->bus().read(Contract::printer_status_address) == 0xFF);
    REQUIRE(machine->bus().read(static_cast<uint16_t>(Contract::io_latch_end_exclusive - 1)) == 0xFF);

    machine->bus().write(Contract::io_latch_start, 0x55);
    REQUIRE(machine->bus().read(Contract::io_latch_start) == 0xFF);
}

TEST_CASE("TRS-80 Model III unmapped keyboard and VRAM read as floating bus 0xFF",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    REQUIRE(machine->bus().read(Contract::keyboard_start) == 0xFF);
    REQUIRE(machine->bus().read(Contract::vram_start) == 0xFF);

    machine->bus().write(Contract::keyboard_start, 0x55);
    REQUIRE(machine->bus().read(Contract::keyboard_start) == 0xFF);
}

TEST_CASE("TRS-80 Model III runs inline NOP HALT program in RAM", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    machine->bus().write(0x4000, 0x00); // NOP
    machine->bus().write(0x4001, 0x76); // HALT
    machine->reset();
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x4000;

    REQUIRE_FALSE(machine->cpu().halted());
    machine->step();
    REQUIRE_FALSE(machine->cpu().halted());
    machine->step();
    REQUIRE(machine->cpu().halted());
}
