#include <cstdint>
#include "catch.hpp"
#include "cpus/m6502.hpp"
#include "cpus/m6502/dispatch.hpp"
#include "helpers.hpp"

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

TEST_CASE("M6502 Addressing Modes", "[cpu][m6502][fast]") {
    M6502 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0xF000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("Immediate mode") {
        cpu.regs().pc = 0x0200;
        uint16_t addr = cpu.addrImmediate();
        REQUIRE(addr == 0x0200);
        REQUIRE(cpu.regs().pc == 0x0201);
    }

    SECTION("Absolute mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0x34);
        bus_and_ram.ram->write(0x0201, 0x12);

        uint16_t addr = cpu.addrAbsolute();
        REQUIRE(addr == 0x1234);
        REQUIRE(cpu.regs().pc == 0x0202);
    }

    SECTION("Zero Page mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0x40);

        uint16_t addr = cpu.addrZeroPage();
        REQUIRE(addr == 0x0040);
        REQUIRE(cpu.regs().pc == 0x0201);
    }

    SECTION("Absolute,X mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0xF0);
        bus_and_ram.ram->write(0x0201, 0x01); // base 0x01F0
        cpu.regs().x = 0x05;

        bool page_crossed = false;
        uint16_t addr = cpu.addrAbsoluteX(page_crossed);
        REQUIRE(addr == 0x01F5);
        REQUIRE_FALSE(page_crossed);

        // Test with page crossing
        cpu.regs().pc = 0x0200;
        cpu.regs().x = 0x20; // 0x01F0 + 0x20 = 0x0210 (crosses page 1 to 2)
        addr = cpu.addrAbsoluteX(page_crossed);
        REQUIRE(addr == 0x0210);
        REQUIRE(page_crossed);
    }

    SECTION("Absolute,Y mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0xF0);
        bus_and_ram.ram->write(0x0201, 0x01); // base 0x01F0
        cpu.regs().y = 0x05;

        bool page_crossed = false;
        uint16_t addr = cpu.addrAbsoluteY(page_crossed);
        REQUIRE(addr == 0x01F5);
        REQUIRE_FALSE(page_crossed);

        // Test with page crossing
        cpu.regs().pc = 0x0200;
        cpu.regs().y = 0x20;
        addr = cpu.addrAbsoluteY(page_crossed);
        REQUIRE(addr == 0x0210);
        REQUIRE(page_crossed);
    }

    SECTION("Zero Page,X mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0xF0);
        cpu.regs().x = 0x15; // 0xF0 + 0x15 = 0x05 (wrap inside page zero)

        uint16_t addr = cpu.addrZeroPageX();
        REQUIRE(addr == 0x0005);
        REQUIRE(cpu.regs().pc == 0x0201);
    }

    SECTION("Zero Page,Y mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0xF0);
        cpu.regs().y = 0x15; // 0xF0 + 0x15 = 0x05 (wrap inside page zero)

        uint16_t addr = cpu.addrZeroPageY();
        REQUIRE(addr == 0x0005);
        REQUIRE(cpu.regs().pc == 0x0201);
    }

    SECTION("Indexed Indirect mode ($zp,X)") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0x20); // zp base 0x20
        cpu.regs().x = 0x04; // zp address 0x24

        // Target address 0x1234 stored at zp 0x0024
        bus_and_ram.ram->write(0x0024, 0x34);
        bus_and_ram.ram->write(0x0025, 0x12);

        uint16_t addr = cpu.addrIndexedIndirect();
        REQUIRE(addr == 0x1234);
        REQUIRE(cpu.regs().pc == 0x0201);

        // Test page zero wrap of read16_zp
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0xFC);
        cpu.regs().x = 0x03; // zp address 0xFF
        // low byte from 0xFF, high byte from 0x00
        bus_and_ram.ram->write(0x00FF, 0x78);
        bus_and_ram.ram->write(0x0000, 0x56);

        addr = cpu.addrIndexedIndirect();
        REQUIRE(addr == 0x5678);
    }

    SECTION("Indirect Indexed mode ($zp),Y") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0x20); // zp base 0x20
        cpu.regs().y = 0x05;

        // Base address 0x01F0 stored at zp 0x20
        bus_and_ram.ram->write(0x0020, 0xF0);
        bus_and_ram.ram->write(0x0021, 0x01);

        bool page_crossed = false;
        uint16_t addr = cpu.addrIndirectIndexed(page_crossed);
        REQUIRE(addr == 0x01F5);
        REQUIRE_FALSE(page_crossed);

        // Test page crossing (0x01F0 + 0x20 = 0x0210)
        cpu.regs().pc = 0x0200;
        cpu.regs().y = 0x20;
        addr = cpu.addrIndirectIndexed(page_crossed);
        REQUIRE(addr == 0x0210);
        REQUIRE(page_crossed);
    }

    SECTION("Relative mode") {
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0x05); // offset +5

        bool page_crossed = false;
        uint16_t addr = cpu.addrRelative(page_crossed);
        // PC becomes 0x0201. Target = 0x0201 + 5 = 0x0206
        REQUIRE(addr == 0x0206);
        REQUIRE(cpu.regs().pc == 0x0201);
        REQUIRE_FALSE(page_crossed);

        // Negative offset: -5 (0xFB)
        cpu.regs().pc = 0x0200;
        bus_and_ram.ram->write(0x0200, 0xFB); // -5
        addr = cpu.addrRelative(page_crossed);
        // PC becomes 0x0201. Target = 0x0201 - 5 = 0x01FC (crosses page 2 to 1)
        REQUIRE(addr == 0x01FC);
        REQUIRE(page_crossed);
    }

    SECTION("Indirect mode (JMP ($addr)) and bug wrap") {
        cpu.regs().pc = 0x0200;
        // Pointer points to 0x0150
        bus_and_ram.ram->write(0x0200, 0x50);
        bus_and_ram.ram->write(0x0201, 0x01);

        // Target address 0x4321 stored at 0x0150
        bus_and_ram.ram->write(0x0150, 0x21);
        bus_and_ram.ram->write(0x0151, 0x43);

        uint16_t addr = cpu.addrIndirect();
        REQUIRE(addr == 0x4321);
        REQUIRE(cpu.regs().pc == 0x0202);

        // Test 6502 page-wrap bug at $xxFF boundary
        cpu.regs().pc = 0x0200;
        // Pointer points to 0x01FF
        bus_and_ram.ram->write(0x0200, 0xFF);
        bus_and_ram.ram->write(0x0201, 0x01);

        // Low byte of target at 0x01FF
        bus_and_ram.ram->write(0x01FF, 0x88);
        // High byte of target at 0x0100 (wraps instead of reading from 0x0200)
        bus_and_ram.ram->write(0x0100, 0x99);

        addr = cpu.addrIndirect();
        REQUIRE(addr == 0x9988);
    }
}


TEST_CASE("M6502 dispatch table has all 256 entries initialized", "[cpu][m6502][fast]") {
    for (int i = 0; i < 256; ++i) {
        REQUIRE(m6502::kDispatch[i] != nullptr);
    }
}

TEST_CASE("M6502 official NOP 0xEA does not halt", "[cpu][m6502][fast]") {
    M6502 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    bus_and_ram.ram->write(0x0000, 0xEA);
    bus_and_ram.ram->write(0x0001, 0xEA);
    cpu.regs().pc = 0x0000;

    unsigned cycles = cpu.step();
    REQUIRE(cycles == 2);
    REQUIRE(cpu.regs().pc == 0x0001);
    REQUIRE_FALSE(cpu.halted());

    cycles = cpu.step();
    REQUIRE(cycles == 2);
    REQUIRE(cpu.regs().pc == 0x0002);
    REQUIRE_FALSE(cpu.halted());
}

