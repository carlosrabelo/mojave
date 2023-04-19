#include <cstdint>
#include "catch.hpp"
#include "cpus/z80.hpp"

TEST_CASE("Z80 reset clears all registers and halts", "[cpu][z80][fast]") {
    Z80 cpu;
    cpu.regs().af = 0x1234;
    cpu.regs().bc = 0xABCD;
    cpu.regs().pc = 0x0100;

    cpu.reset();

    REQUIRE(cpu.regs().af == 0);
    REQUIRE(cpu.regs().bc == 0);
    REQUIRE(cpu.regs().pc == 0);
    REQUIRE_FALSE(cpu.halted());
}

TEST_CASE("Z80 step increments PC by 1", "[cpu][z80][fast]") {
    Z80 cpu;
    cpu.regs().pc = 0x0100;

    unsigned cycles = cpu.step();

    REQUIRE(cycles == 4);
    REQUIRE(cpu.regs().pc == 0x0101);
}

TEST_CASE("Z80 step while halted returns 4 and does not change PC", "[cpu][z80][fast]") {
    Z80 cpu;
    cpu.regs().pc = 0x0100;

    cpu.step();
    REQUIRE(cpu.regs().pc == 0x0101);

    cpu.reset();
    REQUIRE(cpu.regs().pc == 0);
}

TEST_CASE("Z80 registers returns all 14 register entries", "[cpu][z80][fast]") {
    Z80 cpu;
    cpu.regs().af  = 0x0102;
    cpu.regs().bc  = 0x0304;
    cpu.regs().de  = 0x0506;
    cpu.regs().hl  = 0x0708;
    cpu.regs().af_ = 0x1122;
    cpu.regs().bc_ = 0x3344;
    cpu.regs().de_ = 0x5566;
    cpu.regs().hl_ = 0x7788;
    cpu.regs().ix  = 0x090A;
    cpu.regs().iy  = 0x0B0C;
    cpu.regs().sp  = 0x0D0E;
    cpu.regs().pc  = 0x0F10;
    cpu.regs().i   = 0x12;
    cpu.regs().r   = 0x34;

    auto snap = cpu.registers();

    REQUIRE(snap.entries.size() == 14);
    REQUIRE(snap.entries[0].name == "AF");
    REQUIRE(snap.entries[0].value == 0x0102);
    REQUIRE(snap.entries[4].name == "AF'");
    REQUIRE(snap.entries[4].value == 0x1122);
    REQUIRE(snap.entries[12].name == "I");
    REQUIRE(snap.entries[12].value == 0x12);
    REQUIRE(snap.entries[13].name == "R");
    REQUIRE(snap.entries[13].value == 0x34);
}
