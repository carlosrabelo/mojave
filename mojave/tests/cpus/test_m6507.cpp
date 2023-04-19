#include <cstdint>
#include "catch.hpp"
#include "cpus/m6507.hpp"

TEST_CASE("M6507 inherits M6502 registers and behavior", "[cpu][m6507][fast]") {
    M6507 cpu;
    cpu.regs().a = 0x42;
    cpu.regs().pc = 0x0200;

    REQUIRE(cpu.regs().a == 0x42);
    unsigned cycles = cpu.step();
    REQUIRE(cycles == 2);
    REQUIRE(cpu.regs().pc == 0x0201);
}

TEST_CASE("M6507 reset mirrors M6502 reset", "[cpu][m6507][fast]") {
    M6507 cpu;
    cpu.regs().a = 0xFF;
    cpu.regs().p = 0x00;

    cpu.reset();

    REQUIRE(cpu.regs().a == 0);
    REQUIRE(cpu.regs().p == 0x34);
    REQUIRE(cpu.regs().sp == 0xFD);
    REQUIRE_FALSE(cpu.halted());
}

TEST_CASE("M6507 mapAddress masks to 13 bits", "[cpu][m6507][fast]") {
    M6507 cpu;

    REQUIRE(cpu.mapAddress(0x0000) == 0x0000);
    REQUIRE(cpu.mapAddress(0x1FFF) == 0x1FFF);
    REQUIRE(cpu.mapAddress(0x2000) == 0x0000);
    REQUIRE(cpu.mapAddress(0x2345) == 0x0345);
    REQUIRE(cpu.mapAddress(0xFFFF) == 0x1FFF);
    REQUIRE(cpu.mapAddress(0x4000) == 0x0000);
}

TEST_CASE("M6507 registers returns 6 entries like M6502", "[cpu][m6507][fast]") {
    M6507 cpu;
    cpu.regs().a  = 0xAA;
    cpu.regs().pc = 0xEEFF;

    auto snap = cpu.registers();

    REQUIRE(snap.entries.size() == 6);
    REQUIRE(snap.entries[0].name == "A");
    REQUIRE(snap.entries[4].name == "PC");
    REQUIRE(snap.entries[4].value == 0xEEFF);
}
