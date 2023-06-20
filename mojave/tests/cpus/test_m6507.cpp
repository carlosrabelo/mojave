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

#include "helpers.hpp"

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

    SECTION("External interrupts irq() and nmi() are ignored") {
        cpu.reset();
        cpu.regs().sp = 0xFF;
        cpu.regs().pc = 0x0500;
        ram->write(0x0500, 0xEA); // NOP

        // Setup vector at masked 0x1FFE/0x1FFF ($1FFE-$1FFF)
        ram->write(0x1FFE, 0x44);
        ram->write(0x1FFF, 0x33); // Vector $3344

        cpu.setFlagI(false);
        cpu.irq();
        unsigned cycles = cpu.step(); // should run NOP, not IRQ
        REQUIRE_FALSE(cpu.regs().pc == 0x3344);
        REQUIRE(cpu.regs().pc == 0x0501);

        cpu.nmi();
        cycles = cpu.step(); // should run NOP, not NMI
        REQUIRE_FALSE(cpu.regs().pc == 0x1122);
    }

    SECTION("BRK and RTI work with 8 KiB wraps") {
        cpu.reset();
        cpu.regs().sp = 0xFF;
        cpu.regs().pc = 0x0200;
        ram->write(0x0200, 0x00); // BRK

        // Setup IRQ/BRK Vector at masked $1FFE/$1FFF (corresponds to physical $1FFE/$1FFF)
        ram->write(0x1FFE, 0x34);
        ram->write(0x1FFF, 0x12); // Vector $1234

        unsigned cycles = cpu.step(); // Run BRK via step
        REQUIRE(cycles == 7);
        REQUIRE(cpu.regs().pc == 0x1234);

        // Stack has PC high at 0x01FF, PC low at 0x01FE, P at 0x01FD
        uint16_t pushed_pc = ram->read(0x01FE) | (static_cast<uint16_t>(ram->read(0x01FF)) << 8);
        REQUIRE(pushed_pc == 0x0202);

        ram->write(0x01FD, 0x01); // Set Carry
        cycles = cpu.op40(); // RTI
        REQUIRE(cycles == 6);
        REQUIRE(cpu.regs().pc == 0x0202);
        REQUIRE(cpu.regs().p == 0x21);
    }
}

TEST_CASE("M6507 6502 Opcode Integration and Addressing Modes", "[cpu][m6507][fast]") {
    M6507 cpu;
    auto bus = std::make_unique<Bus>();
    auto ram = std::make_unique<Memory>(0x2000); // 8 KiB RAM
    bus->attach(*ram, 0x0000, 0x2000);
    cpu.setBus(bus.get());

    SECTION("Zero-page addressing") {
        cpu.reset();
        ram->write(0x00FF, 0x42);

        // LDA zp ($A5) reading from zp $FF
        ram->write(0x0200, 0xA5);
        ram->write(0x0201, 0xFF);
        cpu.regs().pc = 0x0200;

        cpu.step();
        REQUIRE(cpu.regs().a == 0x42);
    }

    SECTION("Absolute addressing wrap") {
        cpu.reset();
        ram->write(0x0000, 0x99); // Address 0x4000 wraps to 0x0000

        // LDA abs ($AD) reading from $4000 (wraps to 0x0000)
        ram->write(0x0200, 0xAD);
        ram->write(0x0201, 0x00);
        ram->write(0x0202, 0x40);
        cpu.regs().pc = 0x0200;

        cpu.step();
        REQUIRE(cpu.regs().a == 0x99);
    }

    SECTION("Absolute indexed addressing wrap") {
        cpu.reset();
        ram->write(0x0000, 0x77); // Address 0x3FFF + X (X=1) wraps to 0x0000
        cpu.regs().x = 1;

        // LDA abs,X ($BD) reading from $3FFF index X=1 (total 0x4000 -> wraps to 0x0000)
        ram->write(0x0200, 0xBD);
        ram->write(0x0201, 0xFF);
        ram->write(0x0202, 0x3F);
        cpu.regs().pc = 0x0200;

        cpu.step();
        REQUIRE(cpu.regs().a == 0x77);
    }

    SECTION("Indirect JMP wrap") {
        cpu.reset();
        // Setup JMP target (0x5678) at masked 0x3234/0x3235 (which maps to physical 0x1234/0x1235)
        ram->write(0x1234, 0x78); // Target low
        ram->write(0x1235, 0x56); // Target high

        // JMP ($3234) -> opcode 0x6C
        ram->write(0x0200, 0x6C);
        ram->write(0x0201, 0x34);
        ram->write(0x0202, 0x32);
        cpu.regs().pc = 0x0200;

        cpu.step();
        REQUIRE(cpu.regs().pc == 0x5678);
    }
}
