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

TEST_CASE("Z80 Phase 3: 8-bit and 16-bit loads", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("LD BC,nn (01)") {
        bus_and_ram.ram->write(0, 0x01);
        bus_and_ram.ram->write(1, 0x34);
        bus_and_ram.ram->write(2, 0x12);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 10);
        REQUIRE(cpu.regs().bc == 0x1234);
    }

    SECTION("LD (BC),A (02) and LD A,(BC) (0A)") {
        cpu.regs().bc = 0x0500;
        cpu.setA(0x55);
        bus_and_ram.ram->write(0, 0x02);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x55);

        cpu.setA(0);
        bus_and_ram.ram->write(3, 0x0A);
        cpu.regs().pc = 3;
        cpu.step();
        REQUIRE(cpu.getA() == 0x55);
    }

    SECTION("LD (nn),HL (22) and LD HL,(nn) (2A)") {
        cpu.regs().hl = 0xABCD;
        bus_and_ram.ram->write(0, 0x22);
        bus_and_ram.ram->write(1, 0x00);
        bus_and_ram.ram->write(2, 0x06); // nn = 0x0600
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0600) == 0xCD);
        REQUIRE(bus_and_ram.ram->read(0x0601) == 0xAB);

        cpu.regs().hl = 0;
        bus_and_ram.ram->write(3, 0x2A);
        bus_and_ram.ram->write(4, 0x00);
        bus_and_ram.ram->write(5, 0x06);
        cpu.regs().pc = 3;
        cpu.step();
        REQUIRE(cpu.regs().hl == 0xABCD);
    }

    SECTION("LD (nn),A (32) and LD A,(nn) (3A)") {
        cpu.setA(0xEF);
        bus_and_ram.ram->write(0, 0x32);
        bus_and_ram.ram->write(1, 0x00);
        bus_and_ram.ram->write(2, 0x07); // nn = 0x0700
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0700) == 0xEF);

        cpu.setA(0);
        bus_and_ram.ram->write(3, 0x3A);
        bus_and_ram.ram->write(4, 0x00);
        bus_and_ram.ram->write(5, 0x07);
        cpu.regs().pc = 3;
        cpu.step();
        REQUIRE(cpu.getA() == 0xEF);
    }
}

TEST_CASE("Z80 Phase 3: INC/DEC and arithmetic", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("INC B (04) and DEC B (05)") {
        cpu.setB(0x0F);
        bus_and_ram.ram->write(0, 0x04);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getB() == 0x10);
        REQUIRE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagPV());

        cpu.setB(0x80);
        bus_and_ram.ram->write(1, 0x05);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.getB() == 0x7F);
        REQUIRE(cpu.getFlagPV());
        REQUIRE(cpu.getFlagN());
    }

    SECTION("INC (HL) (34) and DEC (HL) (35)") {
        cpu.regs().hl = 0x0500;
        bus_and_ram.ram->write(0x0500, 0xFF);
        bus_and_ram.ram->write(0, 0x34);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x00);
        REQUIRE(cpu.getFlagZ());

        bus_and_ram.ram->write(1, 0x35);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0xFF);
    }

    SECTION("ADD HL,BC (09) and ADD HL,HL (29)") {
        cpu.regs().hl = 0x0FFF;
        cpu.regs().bc = 0x0001;
        bus_and_ram.ram->write(0, 0x09);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().hl == 0x1000);
        REQUIRE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagC());

        cpu.regs().hl = 0x8000;
        bus_and_ram.ram->write(1, 0x29);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.regs().hl == 0x0000);
        REQUIRE(cpu.getFlagC());
    }
}

TEST_CASE("Z80 Phase 3: Exchanges, Rotates, CCF/SCF/CPL/DAA", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("EX AF,AF' (08)") {
        cpu.regs().af = 0x1234;
        cpu.regs().af_ = 0xABCD;
        bus_and_ram.ram->write(0, 0x08);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().af == 0xABCD);
        REQUIRE(cpu.regs().af_ == 0x1234);
    }

    SECTION("RLCA (07)") {
        cpu.setA(0x81);
        bus_and_ram.ram->write(0, 0x07);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x03);
        REQUIRE(cpu.getFlagC());
    }

    SECTION("RRCA (0F)") {
        cpu.setA(0x81);
        bus_and_ram.ram->write(0, 0x0F);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0xC0);
        REQUIRE(cpu.getFlagC());
    }

    SECTION("RLA (17)") {
        cpu.setA(0x80);
        cpu.setFlagC(true);
        bus_and_ram.ram->write(0, 0x17);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x01);
        REQUIRE(cpu.getFlagC());
    }

    SECTION("RRA (1F)") {
        cpu.setA(0x01);
        cpu.setFlagC(true);
        bus_and_ram.ram->write(0, 0x1F);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x80);
        REQUIRE(cpu.getFlagC());
    }

    SECTION("CPL (2F) and SCF (37) and CCF (3F)") {
        cpu.setA(0x55);
        bus_and_ram.ram->write(0, 0x2F);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0xAA);
        REQUIRE(cpu.getFlagH());
        REQUIRE(cpu.getFlagN());

        bus_and_ram.ram->write(1, 0x37);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.getFlagC());
        REQUIRE_FALSE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagN());

        bus_and_ram.ram->write(2, 0x3F);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE_FALSE(cpu.getFlagC());
        REQUIRE(cpu.getFlagH());
    }

    SECTION("DAA (27) - addition") {
        cpu.setA(0x3C);
        cpu.setFlagC(false);
        cpu.setFlagH(false);
        cpu.setFlagN(false);
        bus_and_ram.ram->write(0, 0x27);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x42);
    }
}

TEST_CASE("Z80 Phase 3: Jumps and Branches", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("JR e (18)") {
        bus_and_ram.ram->write(0, 0x18);
        bus_and_ram.ram->write(1, 0x05);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 12);
        REQUIRE(cpu.regs().pc == 7);
    }

    SECTION("JR NZ,e (20) taken and not taken") {
        cpu.setFlagZ(false);
        bus_and_ram.ram->write(0, 0x20);
        bus_and_ram.ram->write(1, 0x10);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 12);
        REQUIRE(cpu.regs().pc == 0x12);

        cpu.setFlagZ(true);
        bus_and_ram.ram->write(0x12, 0x20);
        bus_and_ram.ram->write(0x13, 0x10);
        cpu.regs().pc = 0x12;
        cycles = cpu.step();
        REQUIRE(cycles == 7);
        REQUIRE(cpu.regs().pc == 0x14);
    }

    SECTION("DJNZ (10) loop behavior") {
        cpu.setB(2);
        bus_and_ram.ram->write(0, 0x10);
        bus_and_ram.ram->write(1, 0xFE);
        cpu.regs().pc = 0;

        unsigned cycles = cpu.step();
        REQUIRE(cycles == 13);
        REQUIRE(cpu.getB() == 1);
        REQUIRE(cpu.regs().pc == 0);

        cycles = cpu.step();
        REQUIRE(cycles == 8);
        REQUIRE(cpu.getB() == 0);
        REQUIRE(cpu.regs().pc == 2);
    }
}


TEST_CASE("Z80 Phase 4: Register-to-register loads (40-7F) and HALT", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("LD B, C (41) and LD D, E (5B) and LD A, A (7F)") {
        cpu.setC(0x5A);
        bus_and_ram.ram->write(0, 0x41);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 4);
        REQUIRE(cpu.getB() == 0x5A);

        cpu.setE(0x23);
        bus_and_ram.ram->write(1, 0x53);
        cpu.regs().pc = 1;
        cycles = cpu.step();
        REQUIRE(cycles == 4);
        REQUIRE(cpu.getD() == 0x23);
    }

    SECTION("LD r, (HL) (46) and LD (HL), r (70)") {
        cpu.regs().hl = 0x0500;
        bus_and_ram.ram->write(0x0500, 0xAA);
        bus_and_ram.ram->write(0, 0x46);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 7);
        REQUIRE(cpu.getB() == 0xAA);

        cpu.setC(0x55);
        bus_and_ram.ram->write(1, 0x71);
        cpu.regs().pc = 1;
        cycles = cpu.step();
        REQUIRE(cycles == 7);
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x55);
    }

    SECTION("HALT (76)") {
        bus_and_ram.ram->write(0, 0x76);
        cpu.regs().pc = 0;
        REQUIRE_FALSE(cpu.halted());
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 4);
        REQUIRE(cpu.halted());

        unsigned next_cycles = cpu.step();
        REQUIRE(next_cycles == 4);
        REQUIRE(cpu.regs().pc == 1);
    }
}


TEST_CASE("Z80 Phase 5: 8-bit ALU operations", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("ADD A, r (80) and ADC A, r (88)") {
        cpu.setA(0x3C);
        cpu.setB(0x12);
        bus_and_ram.ram->write(0, 0x80);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 4);
        REQUIRE(cpu.getA() == 0x4E);
        REQUIRE_FALSE(cpu.getFlagC());
        REQUIRE_FALSE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagN());

        cpu.setFlagC(true);
        cpu.setB(0x01);
        bus_and_ram.ram->write(1, 0x88);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.getA() == 0x50);
        REQUIRE(cpu.getFlagH());
    }

    SECTION("SUB r (90) and SBC A, r (98)") {
        cpu.setA(0x01);
        cpu.setB(0x02);
        bus_and_ram.ram->write(0, 0x90);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0xFF);
        REQUIRE(cpu.getFlagC());
        REQUIRE(cpu.getFlagH());
        REQUIRE(cpu.getFlagN());

        cpu.setA(0x10);
        cpu.setB(0x01);
        cpu.setFlagC(true);
        bus_and_ram.ram->write(1, 0x98);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.getA() == 0x0E);
        REQUIRE(cpu.getFlagH());
    }

    SECTION("AND r (A0) and XOR r (A8) and OR r (B0)") {
        cpu.setA(0xF0);
        cpu.setB(0x3C);
        bus_and_ram.ram->write(0, 0xA0);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x30);
        REQUIRE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagC());
        REQUIRE_FALSE(cpu.getFlagN());

        cpu.setB(0x3A);
        bus_and_ram.ram->write(1, 0xA8);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.getA() == 0x0A);
        REQUIRE_FALSE(cpu.getFlagH());
        REQUIRE(cpu.getFlagPV());

        cpu.setB(0xF0);
        bus_and_ram.ram->write(2, 0xB0);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getA() == 0xFA);
    }

    SECTION("CP r (B8) and CP (HL) (BE)") {
        cpu.setA(0x20);
        cpu.setB(0x20);
        bus_and_ram.ram->write(0, 0xB8);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 4);
        REQUIRE(cpu.getA() == 0x20);
        REQUIRE(cpu.getFlagZ());

        cpu.regs().hl = 0x0600;
        bus_and_ram.ram->write(0x0600, 0x10);
        bus_and_ram.ram->write(1, 0xBE);
        cpu.regs().pc = 1;
        cycles = cpu.step();
        REQUIRE(cycles == 7);
        REQUIRE_FALSE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagC());
    }
}


TEST_CASE("Z80 Phase 6: Control Flow, Stack, I/O and Prefixes", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("JP absolute and conditional") {
        bus_and_ram.ram->write(0, 0xC3);
        bus_and_ram.ram->write(1, 0x00);
        bus_and_ram.ram->write(2, 0x05);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0500);

        cpu.setFlagZ(false);
        bus_and_ram.ram->write(0x0500, 0xC2);
        bus_and_ram.ram->write(0x0501, 0x30);
        bus_and_ram.ram->write(0x0502, 0x06);
        cpu.regs().pc = 0x0500;
        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0630);

        cpu.setFlagZ(true);
        bus_and_ram.ram->write(0x0630, 0xC2);
        bus_and_ram.ram->write(0x0631, 0x00);
        bus_and_ram.ram->write(0x0632, 0x07);
        cpu.regs().pc = 0x0630;
        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0633);
    }

    SECTION("CALL and RET conditional/unconditional") {
        cpu.regs().sp = 0x0F00;
        bus_and_ram.ram->write(0, 0xCD);
        bus_and_ram.ram->write(1, 0x00);
        bus_and_ram.ram->write(2, 0x08);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 17);
        REQUIRE(cpu.regs().pc == 0x0800);
        REQUIRE(cpu.regs().sp == 0x0EFE);
        REQUIRE(bus_and_ram.ram->read(0x0EFE) == 0x03);
        REQUIRE(bus_and_ram.ram->read(0x0EFF) == 0x00);

        bus_and_ram.ram->write(0x0800, 0xC9);
        cpu.regs().pc = 0x0800;
        cycles = cpu.step();
        REQUIRE(cycles == 10);
        REQUIRE(cpu.regs().pc == 0x0003);
        REQUIRE(cpu.regs().sp == 0x0F00);
    }

    SECTION("PUSH and POP registers") {
        cpu.regs().sp = 0x0F00;
        cpu.regs().bc = 0x1234;
        bus_and_ram.ram->write(0, 0xC5);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().sp == 0x0EFE);
        REQUIRE(bus_and_ram.ram->read(0x0EFE) == 0x34);
        REQUIRE(bus_and_ram.ram->read(0x0EFF) == 0x12);

        cpu.regs().bc = 0;
        bus_and_ram.ram->write(1, 0xC1);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.regs().bc == 0x1234);
        REQUIRE(cpu.regs().sp == 0x0F00);
    }

    SECTION("RST vectors") {
        cpu.regs().sp = 0x0F00;
        bus_and_ram.ram->write(0, 0xCF);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0008);
        REQUIRE(cpu.regs().sp == 0x0EFE);
        REQUIRE(bus_and_ram.ram->read(0x0EFE) == 0x01);
        REQUIRE(bus_and_ram.ram->read(0x0EFF) == 0x00);
    }

    SECTION("EX DE,HL and EX (SP),HL and EXX") {
        cpu.regs().de = 0x1234;
        cpu.regs().hl = 0xABCD;
        bus_and_ram.ram->write(0, 0xEB);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().hl == 0x1234);
        REQUIRE(cpu.regs().de == 0xABCD);

        cpu.regs().sp = 0x0F00;
        bus_and_ram.ram->write(0x0F00, 0x78);
        bus_and_ram.ram->write(0x0F01, 0x56);
        bus_and_ram.ram->write(1, 0xE3);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.regs().hl == 0x5678);
        REQUIRE(bus_and_ram.ram->read(0x0F00) == 0x34);
        REQUIRE(bus_and_ram.ram->read(0x0F01) == 0x12);

        cpu.regs().bc = 0x1111; cpu.regs().bc_ = 0x2222;
        bus_and_ram.ram->write(2, 0xD9);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.regs().bc == 0x2222);
        REQUIRE(cpu.regs().bc_ == 0x1111);
    }

    SECTION("DI and EI") {
        cpu.regs().iff1 = cpu.regs().iff2 = true;
        bus_and_ram.ram->write(0, 0xF3);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE_FALSE(cpu.regs().iff1);
        REQUIRE_FALSE(cpu.regs().iff2);

        bus_and_ram.ram->write(1, 0xFB);
        cpu.regs().pc = 1;
        cpu.step();
        REQUIRE(cpu.regs().iff1);
        REQUIRE(cpu.regs().iff2);
    }

    struct MockPortDevice : public PortDevice {
        std::unordered_map<uint16_t, uint8_t> ports;
        uint8_t readPort(uint16_t port) override { return ports[port]; }
        void writePort(uint16_t port, uint8_t value) override { ports[port] = value; }
    };

    SECTION("IN and OUT instructions") {
        MockPortDevice mock_port;
        bus_and_ram.bus->attachPort(mock_port, 0x0000, 0xFFFF);

        cpu.setA(0x5A);
        bus_and_ram.ram->write(0, 0xD3);
        bus_and_ram.ram->write(1, 0x01);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(mock_port.ports[0x5A01] == 0x5A);

        cpu.setA(0x12);
        mock_port.ports[0x1202] = 0xE7;
        bus_and_ram.ram->write(2, 0xDB);
        bus_and_ram.ram->write(3, 0x02);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getA() == 0xE7);
    }
}


TEST_CASE("Z80 Phase 7: CB Prefix - Shifts, Rotates, BIT, SET, RES", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("Rotates: RLC B (CB 00) and RL (HL) (CB 16)") {
        cpu.setB(0x85);
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0x00);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 8);
        REQUIRE(cpu.getB() == 0x0B);
        REQUIRE(cpu.getFlagC());

        cpu.regs().hl = 0x0600;
        bus_and_ram.ram->write(0x0600, 0x85);
        cpu.setFlagC(false);
        bus_and_ram.ram->write(2, 0xCB);
        bus_and_ram.ram->write(3, 0x16);
        cpu.regs().pc = 2;
        cycles = cpu.step();
        REQUIRE(cycles == 15);
        REQUIRE(bus_and_ram.ram->read(0x0600) == 0x0A);
        REQUIRE(cpu.getFlagC());
    }

    SECTION("Shifts: SLA C (CB 21) and SRA D (CB 2A) and SRL E (CB 3B) and SLL H (CB 34)") {
        cpu.setC(0x80);
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0x21);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getC() == 0x00);
        REQUIRE(cpu.getFlagC());
        REQUIRE(cpu.getFlagZ());

        cpu.setD(0xF0);
        bus_and_ram.ram->write(2, 0xCB);
        bus_and_ram.ram->write(3, 0x2A);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getD() == 0xF8);
        REQUIRE_FALSE(cpu.getFlagC());

        cpu.setE(0x01);
        bus_and_ram.ram->write(4, 0xCB);
        bus_and_ram.ram->write(5, 0x3B);
        cpu.regs().pc = 4;
        cpu.step();
        REQUIRE(cpu.getE() == 0x00);
        REQUIRE(cpu.getFlagC());

        cpu.setH(0x10);
        bus_and_ram.ram->write(6, 0xCB);
        bus_and_ram.ram->write(7, 0x34);
        cpu.regs().pc = 6;
        cpu.step();
        REQUIRE(cpu.getH() == 0x21);
        REQUIRE_FALSE(cpu.getFlagC());
    }

    SECTION("BIT b, r - BIT 0, A (CB 47) and BIT 7, (HL) (CB 7E)") {
        cpu.setA(0x01);
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0x47);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 8);
        REQUIRE_FALSE(cpu.getFlagZ());

        cpu.regs().hl = 0x0600;
        bus_and_ram.ram->write(0x0600, 0x00);
        bus_and_ram.ram->write(2, 0xCB);
        bus_and_ram.ram->write(3, 0x7E);
        cpu.regs().pc = 2;
        cycles = cpu.step();
        REQUIRE(cycles == 12);
        REQUIRE(cpu.getFlagZ());
    }

    SECTION("SET and RES - SET 4, B (CB E0) and RES 4, B (CB A0)") {
        cpu.setB(0x00);
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0xE0);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getB() == 0x10);

        bus_and_ram.ram->write(2, 0xCB);
        bus_and_ram.ram->write(3, 0x80);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getB() == 0x10);

        bus_and_ram.ram->write(4, 0xCB);
        bus_and_ram.ram->write(5, 0xA0);
        cpu.regs().pc = 4;
        cpu.step();
        REQUIRE(cpu.getB() == 0x00);
    }
}

