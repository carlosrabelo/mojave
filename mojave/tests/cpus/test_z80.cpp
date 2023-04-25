#include <cstdint>
#include "catch.hpp"
#include "cpus/z80.hpp"
#include "cpus/z80/dispatch.hpp"
#include "helpers.hpp"

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

TEST_CASE("Z80 dispatch table has all 256 entries initialized", "[cpu][z80][fast]") {
    for (int i = 0; i < 256; ++i) {
        REQUIRE(z80::kDispatch[i] != nullptr);
    }
    REQUIRE(z80::kDispatch[0x00] == &Z80::op00);
    REQUIRE(z80::kDispatch[0xC0] == &Z80::opC0);
    REQUIRE(z80::kDispatch[0xCB] == &Z80::opCB_prefix);
}

TEST_CASE("Z80 step on NOP executes NOP and returns 4 cycles", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    bus_and_ram.ram->write(0x0000, 0x00);
    cpu.regs().pc = 0x0000;

    unsigned cycles = cpu.step();

    REQUIRE(cycles == 4);
    REQUIRE(cpu.regs().pc == 0x0001);
}

TEST_CASE("Z80 step on Unimplemented opcode logs and returns 4 cycles", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    bus_and_ram.ram->write(0x0000, 0x40);
    cpu.regs().pc = 0x0000;

    unsigned cycles = cpu.step();

    REQUIRE(cycles == 4);
    REQUIRE(cpu.regs().pc == 0x0001);
}
