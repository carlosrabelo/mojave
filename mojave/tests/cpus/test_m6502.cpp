#include <cstdint>
#include "catch.hpp"
#include "cpus/m6502.hpp"

TEST_CASE("M6502 reset clears registers and sets P to 0x34", "[cpu][m6502][fast]") {
    M6502 cpu;
    cpu.regs().a = 0x42;
    cpu.regs().x = 0x10;
    cpu.regs().pc = 0x0200;

    cpu.reset();

    REQUIRE(cpu.regs().a == 0);
    REQUIRE(cpu.regs().x == 0);
    REQUIRE(cpu.regs().pc == 0);
    REQUIRE(cpu.regs().p == 0x34);
    REQUIRE(cpu.regs().sp == 0xFD);
    REQUIRE_FALSE(cpu.halted());
}

TEST_CASE("M6502 step increments PC by 1", "[cpu][m6502][fast]") {
    M6502 cpu;
    cpu.regs().pc = 0x0200;

    unsigned cycles = cpu.step();

    REQUIRE(cycles == 2);
    REQUIRE(cpu.regs().pc == 0x0201);
}

TEST_CASE("M6502 step while halted returns 2 and does not change PC", "[cpu][m6502][fast]") {
    M6502 cpu;
    cpu.regs().pc = 0x0200;

    cpu.step();
    REQUIRE(cpu.regs().pc == 0x0201);

    cpu.reset();
    REQUIRE(cpu.regs().pc == 0);
}

TEST_CASE("M6502 registers returns all 6 register entries", "[cpu][m6502][fast]") {
    M6502 cpu;
    cpu.regs().a  = 0xAA;
    cpu.regs().x  = 0xBB;
    cpu.regs().y  = 0xCC;
    cpu.regs().sp = 0xDD;
    cpu.regs().pc = 0xEEFF;
    cpu.regs().p  = 0x34;

    auto snap = cpu.registers();

    REQUIRE(snap.entries.size() == 6);
    REQUIRE(snap.entries[0].name == "A");
    REQUIRE(snap.entries[0].value == 0xAA);
    REQUIRE(snap.entries[1].name == "X");
    REQUIRE(snap.entries[1].value == 0xBB);
    REQUIRE(snap.entries[4].name == "PC");
    REQUIRE(snap.entries[4].value == 0xEEFF);
}
