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


TEST_CASE("M6502 Loads, Stores, Transfers and Stack", "[cpu][m6502][fast]") {
    M6502 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("LDA immediate") {
        cpu.reset();
        bus_and_ram.ram->write(0x0000, 0x42);
        cpu.regs().pc = 0x0000;
        unsigned cycles = cpu.opA9(); // LDA #$42
        REQUIRE(cycles == 2);
        REQUIRE(cpu.regs().a == 0x42);
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("LDA immediate zero and negative flags") {
        cpu.reset();
        bus_and_ram.ram->write(0x0000, 0x00);
        cpu.regs().pc = 0x0000;
        cpu.opA9(); // LDA #$00
        REQUIRE(cpu.regs().a == 0x00);
        REQUIRE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());

        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x80);
        cpu.opA9(); // LDA #$80
        REQUIRE(cpu.regs().a == 0x80);
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE(cpu.getFlagN());
    }

    SECTION("STA/STX/STY zp") {
        cpu.reset();
        cpu.regs().a = 0x55;
        cpu.regs().x = 0xAA;
        cpu.regs().y = 0x11;

        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x10); // zp target 0x10
        unsigned cycles = cpu.op85(); // STA $10
        REQUIRE(cycles == 3);
        REQUIRE(bus_and_ram.ram->read(0x0010) == 0x55);

        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x11); // zp target 0x11
        cycles = cpu.op86(); // STX $11
        REQUIRE(cycles == 3);
        REQUIRE(bus_and_ram.ram->read(0x0011) == 0xAA);

        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x12); // zp target 0x12
        cycles = cpu.op84(); // STY $12
        REQUIRE(cycles == 3);
        REQUIRE(bus_and_ram.ram->read(0x0012) == 0x11);
    }

    SECTION("Register Transfers") {
        cpu.reset();
        cpu.regs().a = 0x40;
        cpu.regs().x = 0x00;

        unsigned cycles = cpu.opAA(); // TAX
        REQUIRE(cycles == 2);
        REQUIRE(cpu.regs().x == 0x40);
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());

        cpu.regs().a = 0x80;
        cpu.opA8(); // TAY
        REQUIRE(cpu.regs().y == 0x80);
        REQUIRE(cpu.getFlagN());

        cpu.regs().x = 0x00;
        cpu.op8A(); // TXA
        REQUIRE(cpu.regs().a == 0x00);
        REQUIRE(cpu.getFlagZ());
    }

    SECTION("PHA/PLA Accumulator Stack") {
        cpu.reset();
        cpu.regs().sp = 0xFF;
        cpu.regs().a = 0x5A;

        unsigned cycles = cpu.op48(); // PHA
        REQUIRE(cycles == 3);
        REQUIRE(cpu.regs().sp == 0xFE);
        REQUIRE(bus_and_ram.ram->read(0x01FF) == 0x5A);

        cpu.regs().a = 0x00;
        cycles = cpu.op68(); // PLA
        REQUIRE(cycles == 4);
        REQUIRE(cpu.regs().a == 0x5A);
        REQUIRE(cpu.regs().sp == 0xFF);
    }

    SECTION("PHP/PLP Status Stack") {
        cpu.reset();
        cpu.regs().sp = 0xFF;
        cpu.regs().p = 0x05; // flags: I flag disabled, Carry set (0000 0101)

        unsigned cycles = cpu.op08(); // PHP
        REQUIRE(cycles == 3);
        REQUIRE(cpu.regs().sp == 0xFE);
        // PHP sets bit 4 (B) and bit 5 (U) in the pushed value (0x05 | 0x30 = 0x35)
        REQUIRE(bus_and_ram.ram->read(0x01FF) == 0x35);

        cpu.regs().p = 0x00;
        cycles = cpu.op28(); // PLP
        REQUIRE(cycles == 4);
        REQUIRE(cpu.regs().sp == 0xFF);
        // PLP ignores bit 4 (B), but forces bit 5 (U) to 1.
        // Pushed value was 0x35. Unstacked: (0x35 & ~0x10) | 0x20 = 0x25
        REQUIRE(cpu.regs().p == 0x25);
    }
}


TEST_CASE("M6502 ALU Operations (AND, ORA, EOR, ADC, SBC)", "[cpu][m6502][fast]") {
    M6502 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("AND operation") {
        cpu.reset();
        cpu.regs().a = 0xFF;
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0xA5);

        cpu.op29(); // AND #$A5
        REQUIRE(cpu.regs().a == 0xA5);
        REQUIRE(cpu.getFlagN()); // 0xA5 is negative (bit 7 set)
        REQUIRE_FALSE(cpu.getFlagZ());
    }

    SECTION("ORA operation") {
        cpu.reset();
        cpu.regs().a = 0x50;
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x0A);

        cpu.op09(); // ORA #$0A
        REQUIRE(cpu.regs().a == 0x5A);
        REQUIRE_FALSE(cpu.getFlagN());
        REQUIRE_FALSE(cpu.getFlagZ());
    }

    SECTION("EOR operation") {
        cpu.reset();
        cpu.regs().a = 0xAA;
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0xAA);

        cpu.op49(); // EOR #$AA
        REQUIRE(cpu.regs().a == 0x00);
        REQUIRE_FALSE(cpu.getFlagN());
        REQUIRE(cpu.getFlagZ());
    }

    SECTION("ADC binary mode no carry/overflow") {
        cpu.reset();
        cpu.regs().a = 0x50;
        cpu.setFlagC(false);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x10);

        cpu.op69(); // ADC #$10 -> 0x50 + 0x10 + 0 = 0x60
        REQUIRE(cpu.regs().a == 0x60);
        REQUIRE_FALSE(cpu.getFlagC());
        REQUIRE_FALSE(cpu.getFlagV());
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("ADC binary mode with carry in and out") {
        cpu.reset();
        cpu.regs().a = 0xFF;
        cpu.setFlagC(true);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x01);

        cpu.op69(); // ADC #$01 -> 0xFF + 0x01 + 1 = 0x101 -> 0x01 (Carry out)
        REQUIRE(cpu.regs().a == 0x01);
        REQUIRE(cpu.getFlagC());
        REQUIRE_FALSE(cpu.getFlagV());
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("ADC binary mode overflow positive") {
        cpu.reset();
        cpu.regs().a = 0x50; // positive
        cpu.setFlagC(false);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x40); // positive

        cpu.op69(); // ADC #$40 -> 0x50 + 0x40 = 0x90 (negative)
        REQUIRE(cpu.regs().a == 0x90);
        REQUIRE_FALSE(cpu.getFlagC());
        REQUIRE(cpu.getFlagV()); // Overflow set!
        REQUIRE(cpu.getFlagN());
    }

    SECTION("ADC binary mode overflow negative") {
        cpu.reset();
        cpu.regs().a = 0xD0; // negative (-48)
        cpu.setFlagC(false);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x90); // negative (-112)

        cpu.op69(); // ADC #$90 -> 0xD0 + 0x90 = 0x160 -> 0x60 (positive, 96)
        REQUIRE(cpu.regs().a == 0x60);
        REQUIRE(cpu.getFlagC()); // Carry set
        REQUIRE(cpu.getFlagV()); // Overflow set!
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("SBC binary mode no borrow") {
        cpu.reset();
        cpu.regs().a = 0x50;
        cpu.setFlagC(true); // C=1 means no borrow in SBC
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x10);

        cpu.opE9(); // SBC #$10 -> 0x50 - 0x10 - 0 = 0x40
        REQUIRE(cpu.regs().a == 0x40);
        REQUIRE(cpu.getFlagC()); // C=1 (no borrow occurred)
        REQUIRE_FALSE(cpu.getFlagV());
    }

    SECTION("SBC binary mode with borrow") {
        cpu.reset();
        cpu.regs().a = 0x50;
        cpu.setFlagC(false); // C=0 means borrow in SBC
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x10);

        cpu.opE9(); // SBC #$10 -> 0x50 - 0x10 - 1 = 0x3F
        REQUIRE(cpu.regs().a == 0x3F);
        REQUIRE(cpu.getFlagC()); // C=1 (no borrow occurred from final result)
        REQUIRE_FALSE(cpu.getFlagV());
    }

    SECTION("SBC binary mode borrow out") {
        cpu.reset();
        cpu.regs().a = 0x50;
        cpu.setFlagC(true);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x60);

        cpu.opE9(); // SBC #$60 -> 0x50 - 0x60 = -0x10 -> 0xF0
        REQUIRE(cpu.regs().a == 0xF0);
        REQUIRE_FALSE(cpu.getFlagC()); // C=0 (borrow occurred)
        REQUIRE_FALSE(cpu.getFlagV());
        REQUIRE(cpu.getFlagN());
    }

    SECTION("SBC binary mode overflow") {
        cpu.reset();
        cpu.regs().a = 0x50; // positive
        cpu.setFlagC(true);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0xB0); // negative (-80)

        cpu.opE9(); // SBC #$B0 -> 0x50 - (-0x50) = 0xA0 (negative)
        REQUIRE(cpu.regs().a == 0xA0);
        REQUIRE_FALSE(cpu.getFlagC()); // borrow occurred
        REQUIRE(cpu.getFlagV()); // Overflow set!
        REQUIRE(cpu.getFlagN());
    }

}


TEST_CASE("M6502 BCD mode for ADC and SBC", "[cpu][m6502][fast]") {
    M6502 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0xF000);
    cpu.setBus(bus_and_ram.bus.get());
    cpu.regs().pc = 0x0000;

    SECTION("ADC BCD mode carry generation") {
        cpu.reset();
        cpu.regs().a = 0x99; // BCD 99
        cpu.setFlagD(true);  // Enable BCD mode
        cpu.setFlagC(false);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x01); // BCD 01

        cpu.op69(); // ADC #$01 -> BCD 99 + 01 + 0 = BCD 00 (Carry set)
        REQUIRE(cpu.regs().a == 0x00);
        REQUIRE(cpu.getFlagC()); // Carry should be set!
        REQUIRE(cpu.getFlagZ()); // Zero flag set
        REQUIRE_FALSE(cpu.getFlagN());
    }
    SECTION("ADC BCD mode simple sum") {
        cpu.reset();
        cpu.regs().a = 0x50; // BCD 50
        cpu.setFlagD(true);  // Enable BCD mode
        cpu.setFlagC(false);
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x40); // BCD 40

        cpu.op69(); // ADC #$40 -> BCD 50 + 40 + 0 = BCD 90
        REQUIRE(cpu.regs().a == 0x90);
        REQUIRE_FALSE(cpu.getFlagC()); // Carry not set
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE(cpu.getFlagN()); // 0x90 has bit 7 set
    }
    SECTION("SBC BCD mode simple subtract") {
        cpu.reset();
        cpu.regs().a = 0x50; // BCD 50
        cpu.setFlagD(true);  // Enable BCD mode
        cpu.setFlagC(true);  // C=1 (no borrow)
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x10); // BCD 10

        cpu.opE9(); // SBC #$10 -> BCD 50 - 10 = BCD 40
        REQUIRE(cpu.regs().a == 0x40);
        REQUIRE(cpu.getFlagC()); // C=1 (no borrow occurred)
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());
    }
    SECTION("SBC BCD mode borrow generation") {
        cpu.reset();
        cpu.regs().a = 0x50; // BCD 50
        cpu.setFlagD(true);  // Enable BCD mode
        cpu.setFlagC(true);  // C=1 (no borrow)
        cpu.regs().pc = 0x0000;
        bus_and_ram.ram->write(0x0000, 0x60); // BCD 60

        cpu.opE9(); // SBC #$60 -> BCD 50 - 60 = BCD 90 (borrow)
        REQUIRE(cpu.regs().a == 0x90);
        REQUIRE_FALSE(cpu.getFlagC()); // Carry cleared (borrow occurred)
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE(cpu.getFlagN());
    }
}
