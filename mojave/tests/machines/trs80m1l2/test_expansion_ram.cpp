#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"

using Contract = Trs80M1L2PresetContract;

TEST_CASE("TRS-80 Model I Level II expansion RAM contract covers 32 KiB at 0x8000",
          "[machine][trs80m1l2][fast]") {
    REQUIRE(Contract::expansion_ram_bytes == 32768u);
    REQUIRE(Contract::expansion_ram_start == 0x8000);
    REQUIRE(Contract::expansion_ram_end_exclusive == 65536u);
    REQUIRE(Contract::ram_end_exclusive == Contract::expansion_ram_start);
}

TEST_CASE("TRS-80 Model I Level II base and expansion RAM are independent at 0x7FFF",
          "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    machine->bus().write(0x7FFF, 0xAA);
    machine->bus().write(0x8000, 0xBB);

    REQUIRE(machine->bus().read(0x7FFF) == 0xAA);
    REQUIRE(machine->bus().read(0x8000) == 0xBB);

    machine->bus().write(0x7FFF, 0xCC);
    REQUIRE(machine->bus().read(0x8000) == 0xBB);
}

TEST_CASE("TRS-80 Model I Level II expansion RAM is writable across the full 32 KiB window",
          "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    const uint16_t probes[] = {0x8000, 0xA000, 0xBFFF, 0xC000, 0xFFFE, 0xFFFF};
    for (uint16_t addr : probes) {
        const uint8_t value = static_cast<uint8_t>(addr & 0xFFu);
        machine->bus().write(addr, value);
        REQUIRE(machine->bus().read(addr) == value);
    }
}

TEST_CASE("TRS-80 Model I Level II runs inline NOP HALT program in expansion RAM",
          "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    machine->bus().write(0x8000, 0x00); // NOP
    machine->bus().write(0x8001, 0x76); // HALT
    machine->reset();
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x8000;

    REQUIRE_FALSE(machine->cpu().halted());
    machine->step();
    REQUIRE_FALSE(machine->cpu().halted());
    machine->step();
    REQUIRE(machine->cpu().halted());
}
