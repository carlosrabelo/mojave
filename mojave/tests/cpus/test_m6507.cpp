#include <cstdint>
#include "catch.hpp"
#include "cpus/m6507.hpp"
#include "helpers.hpp"

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

TEST_CASE("M6507 8 KiB Wrap and Interrupt Behavior", "[cpu][m6507][fast]") {
    M6507 cpu;
    auto bus = std::make_unique<Bus>();
    auto ram = std::make_unique<Memory>(0x2000); // 8 KiB RAM (0x0000 - 0x1FFF)
    bus->attach(*ram, 0x0000, 0x2000);
    cpu.setBus(bus.get());

    SECTION("Unofficial LAX zp works in the 8 KiB mirror") {
        cpu.reset();
        ram->write(0x0010, 0x55);
        ram->write(0x0200, 0xA7);
        ram->write(0x0201, 0x10);
        cpu.regs().pc = 0x0200;
        REQUIRE(cpu.step() == 3);
        REQUIRE(cpu.regs().a == 0x55);
        REQUIRE(cpu.regs().x == 0x55);
        REQUIRE_FALSE(cpu.halted());
    }

    SECTION("Word read at 0x1FFF wraps correctly") {
        cpu.reset();
        ram->write(0x1FFF, 0x34);
        ram->write(0x0000, 0x12); // Wrapped high byte

        uint16_t val = cpu.read16(0x1FFF);
        REQUIRE(val == 0x1234);
    }

    SECTION("Stack operations stay within valid mirror range") {
        cpu.reset();
        cpu.regs().sp = 0xFF; // Stack starts at 0x01FF

        cpu.push8(0xAA);
        REQUIRE(ram->read(0x01FF) == 0xAA);
        REQUIRE(cpu.pop8() == 0xAA);
    }

    SECTION("Instruction fetch wraps when PC crosses 0x1FFF") {
        cpu.reset();
        cpu.regs().pc = 0x1FFF;
        ram->write(0x1FFF, 0xEA); // NOP at 0x1FFF
        ram->write(0x0000, 0x38); // SEC at 0x2000 (masked to 0x0000)

        // Step 1: execute NOP at 0x1FFF, PC increments to 0x2000
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 2);
        REQUIRE(cpu.regs().pc == 0x2000);
        REQUIRE_FALSE(cpu.halted());

        // Step 2: execute SEC at 0x2000 (which reads 0x0000)
        cycles = cpu.step();
        REQUIRE(cycles == 2);
        REQUIRE(cpu.regs().pc == 0x2001);
        REQUIRE(cpu.getFlagC()); // SEC was executed
        REQUIRE_FALSE(cpu.halted());
    }

}

