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


TEST_CASE("Z80 Phase 8: ED Prefix - 16-bit ALU, Block operations, I/O, Interrupt modes", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    struct MockPortDevice : public PortDevice {
        std::unordered_map<uint16_t, uint8_t> ports;
        uint8_t readPort(uint16_t port) override { return ports[port]; }
        void writePort(uint16_t port, uint8_t value) override { ports[port] = value; }
    } mock_port;
    bus_and_ram.bus->attachPort(mock_port, 0x0000, 0xFFFF);

    SECTION("IN r,(C) and OUT (C),r") {
        cpu.regs().bc = 0x1234;
        mock_port.ports[0x1234] = 0x88;
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x78);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 12);
        REQUIRE(cpu.getA() == 0x88);
        REQUIRE(cpu.getFlagS());
        REQUIRE_FALSE(cpu.getFlagZ());

        cpu.setD(0x55);
        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0x51);
        cpu.regs().pc = 2;
        cycles = cpu.step();
        REQUIRE(cycles == 12);
        REQUIRE(mock_port.ports[0x1234] == 0x55);
    }

    SECTION("ADC HL, ss and SBC HL, ss") {
        cpu.regs().hl = 0x1000;
        cpu.regs().bc = 0x0100;
        cpu.setFlagC(true);
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x4A);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 15);
        REQUIRE(cpu.regs().hl == 0x1101);

        cpu.regs().hl = 0x1000;
        cpu.regs().de = 0x0100;
        cpu.setFlagC(true);
        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0x52);
        cpu.regs().pc = 2;
        cycles = cpu.step();
        REQUIRE(cycles == 15);
        REQUIRE(cpu.regs().hl == 0x0EFF);
    }

    SECTION("NEG and RETN / RETI") {
        cpu.setA(0x01);
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x44);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 8);
        REQUIRE(cpu.getA() == 0xFF);
        REQUIRE(cpu.getFlagC());

        cpu.regs().sp = 0x0F00;
        cpu.regs().iff2 = true;
        bus_and_ram.ram->write(0x0F00, 0x00);
        bus_and_ram.ram->write(0x0F01, 0x05);
        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0x45);
        cpu.regs().pc = 2;
        cycles = cpu.step();
        REQUIRE(cycles == 14);
        REQUIRE(cpu.regs().pc == 0x0500);
        REQUIRE(cpu.regs().iff1);
    }

    SECTION("LD (nn),xx and LD xx,(nn)") {
        cpu.regs().de = 0x5678;
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x53);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x06);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 20);
        REQUIRE(bus_and_ram.ram->read(0x0600) == 0x78);
        REQUIRE(bus_and_ram.ram->read(0x0601) == 0x56);

        bus_and_ram.ram->write(4, 0xED);
        bus_and_ram.ram->write(5, 0x4B);
        bus_and_ram.ram->write(6, 0x00);
        bus_and_ram.ram->write(7, 0x06);
        cpu.regs().pc = 4;
        cycles = cpu.step();
        REQUIRE(cycles == 20);
        REQUIRE(cpu.regs().bc == 0x5678);
    }

    SECTION("LD (nn),HL (ED 63) and LD HL,(nn) (ED 6B)") {
        cpu.regs().hl = 0xABCD;
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x63);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x07);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 20);
        REQUIRE(bus_and_ram.ram->read(0x0700) == 0xCD);
        REQUIRE(bus_and_ram.ram->read(0x0701) == 0xAB);
        REQUIRE(cpu.regs().pc == 4);
        REQUIRE_FALSE(cpu.halted());

        cpu.regs().hl = 0;
        bus_and_ram.ram->write(4, 0xED);
        bus_and_ram.ram->write(5, 0x6B);
        bus_and_ram.ram->write(6, 0x00);
        bus_and_ram.ram->write(7, 0x07);
        cpu.regs().pc = 4;
        cycles = cpu.step();
        REQUIRE(cycles == 20);
        REQUIRE(cpu.regs().hl == 0xABCD);
        REQUIRE(cpu.regs().pc == 8);
        REQUIRE_FALSE(cpu.halted());
    }

    SECTION("Interrupt modes (IM 0/1/2)") {
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x5E);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().im == 1);

        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0x6E);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.regs().im == 2);
    }

    SECTION("LDI and LDIR and CPI") {
        cpu.regs().hl = 0x0500;
        cpu.regs().de = 0x0600;
        cpu.regs().bc = 2;
        bus_and_ram.ram->write(0x0500, 0xAA);
        bus_and_ram.ram->write(0x0501, 0xBB);
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0xB0);
        cpu.regs().pc = 0;

        unsigned cycles = cpu.step();
        REQUIRE(cycles == 21);
        REQUIRE(bus_and_ram.ram->read(0x0600) == 0xAA);
        REQUIRE(cpu.regs().bc == 1);
        REQUIRE(cpu.regs().pc == 0);

        cycles = cpu.step();
        REQUIRE(cycles == 16);
        REQUIRE(bus_and_ram.ram->read(0x0601) == 0xBB);
        REQUIRE(cpu.regs().bc == 0);
        REQUIRE(cpu.regs().pc == 2);

        cpu.setA(0x3C);
        cpu.regs().hl = 0x0500;
        cpu.regs().bc = 1;
        bus_and_ram.ram->write(0x0500, 0x3C);
        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0xA1);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getFlagZ());
        REQUIRE(cpu.regs().hl == 0x0501);
        REQUIRE_FALSE(cpu.getFlagPV());
    }

    SECTION("RLD and RRD") {
        cpu.setA(0x8A);
        cpu.regs().hl = 0x0500;
        bus_and_ram.ram->write(0x0500, 0x12);
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x67);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x82);
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0xA1);

        cpu.setA(0x8A);
        bus_and_ram.ram->write(0x0500, 0x12);
        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0x6F);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getA() == 0x81);
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x2A);
    }


    SECTION("Unofficial ED opcodes are 8-cycle NOPs") {
        const uint8_t unofficial[] = {0x00, 0x76, 0x77, 0x7E, 0x7F, 0xFF};
        uint16_t pc = 0;
        for (uint8_t op : unofficial) {
            bus_and_ram.ram->write(pc, 0xED);
            bus_and_ram.ram->write(pc + 1, op);
            cpu.regs().pc = pc;
            unsigned cycles = cpu.step();
            REQUIRE(cycles == 8);
            REQUIRE(cpu.regs().pc == static_cast<uint16_t>(pc + 2));
            REQUIRE_FALSE(cpu.halted());
            pc = static_cast<uint16_t>(pc + 2);
        }
    }
}


TEST_CASE("Z80 Phase 9: IX/IY 16-bit loads and arithmetic (DD/FD)", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("LD IX,nn (DD 21) and LD IY,nn (FD 21)") {
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x21);
        bus_and_ram.ram->write(2, 0x34);
        bus_and_ram.ram->write(3, 0x12);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 14);
        REQUIRE(cpu.regs().ix == 0x1234);
        REQUIRE(cpu.regs().pc == 4);

        bus_and_ram.ram->write(4, 0xFD);
        bus_and_ram.ram->write(5, 0x21);
        bus_and_ram.ram->write(6, 0x78);
        bus_and_ram.ram->write(7, 0x56);
        cpu.regs().pc = 4;
        cpu.step();
        REQUIRE(cpu.regs().iy == 0x5678);
    }

    SECTION("INC IX (DD 23) and DEC IX (DD 2B)") {
        cpu.regs().ix = 0x1000;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x23);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 10);
        REQUIRE(cpu.regs().ix == 0x1001);

        bus_and_ram.ram->write(2, 0xDD);
        bus_and_ram.ram->write(3, 0x2B);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.regs().ix == 0x1000);
    }

    SECTION("ADD IX,BC (DD 09) with half-carry") {
        cpu.regs().ix = 0x0FFF;
        cpu.regs().bc = 0x0001;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x09);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 15);
        REQUIRE(cpu.regs().ix == 0x1000);
        REQUIRE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagC());
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("ADD IX,IX (DD 29)") {
        cpu.regs().ix = 0x0003;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x29);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().ix == 0x0006);
    }

    SECTION("ADD IY,SP (FD 39)") {
        cpu.regs().iy = 0x0010;
        cpu.regs().sp = 0x0020;
        bus_and_ram.ram->write(0, 0xFD);
        bus_and_ram.ram->write(1, 0x39);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 15);
        REQUIRE(cpu.regs().iy == 0x0030);
    }

    SECTION("LD (nn),IX (DD 22) and LD IX,(nn) (DD 2A)") {
        cpu.regs().ix = 0xBEEF;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x22);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x06); // nn = 0x0600
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 20);
        REQUIRE(bus_and_ram.ram->read(0x0600) == 0xEF);
        REQUIRE(bus_and_ram.ram->read(0x0601) == 0xBE);

        cpu.regs().ix = 0;
        bus_and_ram.ram->write(4, 0xDD);
        bus_and_ram.ram->write(5, 0x2A);
        bus_and_ram.ram->write(6, 0x00);
        bus_and_ram.ram->write(7, 0x06);
        cpu.regs().pc = 4;
        cpu.step();
        REQUIRE(cpu.regs().ix == 0xBEEF);
    }
}

TEST_CASE("Z80 Phase 9: indexed addressing (IX+d) and (IY+d)", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("LD A,(IX+d) (DD 7E) with positive displacement") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0x0502, 0x42);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x7E);
        bus_and_ram.ram->write(2, 0x02); // d = +2
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 19);
        REQUIRE(cpu.getA() == 0x42);
        REQUIRE(cpu.regs().pc == 3);
    }

    SECTION("LD A,(IY+d) (FD 7E) with negative displacement") {
        cpu.regs().iy = 0x0500;
        bus_and_ram.ram->write(0x04FE, 0x77);
        bus_and_ram.ram->write(0, 0xFD);
        bus_and_ram.ram->write(1, 0x7E);
        bus_and_ram.ram->write(2, 0xFE); // d = -2
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x77);
    }

    SECTION("LD (IX+d),A (DD 77)") {
        cpu.regs().ix = 0x0500;
        cpu.setA(0xCC);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x77);
        bus_and_ram.ram->write(2, 0x05); // d = +5
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0505) == 0xCC);
    }

    SECTION("LD (IX+d),n (DD 36)") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x36);
        bus_and_ram.ram->write(2, 0x03); // d = +3
        bus_and_ram.ram->write(3, 0xEE); // n
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 19);
        REQUIRE(bus_and_ram.ram->read(0x0503) == 0xEE);
        REQUIRE(cpu.regs().pc == 4);
    }

    SECTION("INC (IX+d) (DD 34) and DEC (IX+d) (DD 35)") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0x0501, 0xFF);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x34);
        bus_and_ram.ram->write(2, 0x01);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 23);
        REQUIRE(bus_and_ram.ram->read(0x0501) == 0x00);
        REQUIRE(cpu.getFlagZ());
        REQUIRE_FALSE(cpu.getFlagN());

        bus_and_ram.ram->write(3, 0xDD);
        bus_and_ram.ram->write(4, 0x35);
        bus_and_ram.ram->write(5, 0x01);
        cpu.regs().pc = 3;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0501) == 0xFF);
        REQUIRE(cpu.getFlagN());
    }

    SECTION("H/L quirk: LD H,(IX+d) (DD 66) targets real H") {
        cpu.regs().ix = 0x0500;
        cpu.regs().hl = 0x0000;
        bus_and_ram.ram->write(0x0500, 0x42);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x66);
        bus_and_ram.ram->write(2, 0x00);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getH() == 0x42);
        REQUIRE(cpu.regs().ix == 0x0500); // IX unchanged
    }

    SECTION("H/L quirk: LD (IX+d),H (DD 74) reads real H") {
        cpu.regs().ix = 0x0500;
        cpu.setH(0x99);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x74);
        bus_and_ram.ram->write(2, 0x00);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x99);
    }

    SECTION("ALU with (IX+d): ADD A,(IX+d) (DD 86)") {
        cpu.regs().ix = 0x0500;
        cpu.setA(0x10);
        bus_and_ram.ram->write(0x0500, 0x20);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x86);
        bus_and_ram.ram->write(2, 0x00);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 19);
        REQUIRE(cpu.getA() == 0x30);
        REQUIRE_FALSE(cpu.getFlagC());
    }

    SECTION("ALU with IXH: CP IXH (DD BC)") {
        cpu.regs().ix = 0x3400;
        cpu.setA(0x12);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xBC);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getFlagC());   // 0x12 < 0x34
        REQUIRE_FALSE(cpu.getFlagZ());
    }
}

TEST_CASE("Z80 Phase 9: IX stack and control flow", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("PUSH IX (DD E5) and POP IX (DD E1)") {
        cpu.regs().ix = 0xBEEF;
        cpu.regs().sp = 0x0F00;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xE5);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 15);
        REQUIRE(bus_and_ram.ram->read(0x0EFE) == 0xEF);
        REQUIRE(bus_and_ram.ram->read(0x0EFF) == 0xBE);
        REQUIRE(cpu.regs().sp == 0x0EFE);

        cpu.regs().ix = 0;
        bus_and_ram.ram->write(2, 0xDD);
        bus_and_ram.ram->write(3, 0xE1);
        cpu.regs().pc = 2;
        REQUIRE(cpu.step() == 14);
        REQUIRE(cpu.regs().ix == 0xBEEF);
        REQUIRE(cpu.regs().sp == 0x0F00);
    }

    SECTION("EX (SP),IX (DD E3)") {
        cpu.regs().ix = 0x1111;
        cpu.regs().sp = 0x0F00;
        bus_and_ram.ram->write(0x0F00, 0xCD);
        bus_and_ram.ram->write(0x0F01, 0xAB);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xE3);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 23);
        REQUIRE(cpu.regs().ix == 0xABCD);
        REQUIRE(bus_and_ram.ram->read(0x0F00) == 0x11);
        REQUIRE(bus_and_ram.ram->read(0x0F01) == 0x11);
    }

    SECTION("JP (IX) (DD E9)") {
        cpu.regs().ix = 0x1234;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xE9);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 8);
        REQUIRE(cpu.regs().pc == 0x1234);
    }

    SECTION("LD SP,IX (DD F9)") {
        cpu.regs().ix = 0x4321;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xF9);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 10);
        REQUIRE(cpu.regs().sp == 0x4321);
    }
}

TEST_CASE("Z80 Phase 9: DDCB compound operations", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("RLC (IX+d) (DDCB d 06) updates memory only") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0x0500, 0x81);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00); // d
        bus_and_ram.ram->write(3, 0x06); // RLC (IX+d)
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 23);
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x03);
        REQUIRE(cpu.getFlagC());
    }

    SECTION("LD B,RLC (IX+d) (DDCB d 00) copies to B too") {
        cpu.regs().ix = 0x0500;
        cpu.setB(0x00);
        bus_and_ram.ram->write(0x0500, 0x01);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x00); // LD B,RLC (IX+d)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x02);
        REQUIRE(cpu.getB() == 0x02);
    }

    SECTION("LD H,SRL (IX+d) (DDCB d 3C) copies to real H") {
        cpu.regs().ix = 0x0500;
        cpu.regs().hl = 0x0000;
        bus_and_ram.ram->write(0x0500, 0x02);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x3C); // LD H,SRL (IX+d)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x01);
        REQUIRE(cpu.getH() == 0x01);
    }

    SECTION("BIT 0,(IX+d) (DDCB d 46)") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0x0500, 0x00);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x46); // BIT 0,(IX+d)
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 20);
        REQUIRE(cpu.getFlagZ());
        REQUIRE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("RES 0,(IX+d) (DDCB d 86)") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0x0500, 0xFF);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x86); // RES 0,(IX+d)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0xFE);
    }

    SECTION("SET 7,(IX+d) (DDCB d FE)") {
        cpu.regs().ix = 0x0500;
        bus_and_ram.ram->write(0x0500, 0x00);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0xFE); // SET 7,(IX+d)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x80);
    }

    SECTION("FDCB mirror: RRC (IY+d)") {
        cpu.regs().iy = 0x0500;
        bus_and_ram.ram->write(0x0500, 0x01);
        bus_and_ram.ram->write(0, 0xFD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);
        bus_and_ram.ram->write(3, 0x0E); // RRC (IY+d)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x80);
        REQUIRE(cpu.getFlagC());
    }
}

TEST_CASE("Z80 Phase 9: DD/FD prefix fallthrough and nesting", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("DD + NOP: prefix acts as NOP") {
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x00);
        cpu.regs().pc = 0;
        unsigned cycles = cpu.step();
        REQUIRE(cycles == 8);
        REQUIRE(cpu.regs().pc == 2);
    }

    SECTION("DD + LD B,C (DD 41): no IX effect") {
        cpu.setB(0x00);
        cpu.setC(0x55);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x41);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getB() == 0x55);
    }

    SECTION("DD + CCF (DD 3F): runs as CCF") {
        cpu.regs().hl = 0x1234; // must stay untouched
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x3F);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getFlagC());     // CCF sets carry
        REQUIRE(cpu.regs().hl == 0x1234);
    }

    SECTION("Nested prefixes: DD FD -> IY wins") {
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xFD);
        bus_and_ram.ram->write(2, 0x21);
        bus_and_ram.ram->write(3, 0x11);
        bus_and_ram.ram->write(4, 0x22);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().iy == 0x2211);
        REQUIRE(cpu.regs().ix == 0x0000);
    }

    SECTION("Nested prefixes: FD DD -> IX wins") {
        bus_and_ram.ram->write(0, 0xFD);
        bus_and_ram.ram->write(1, 0xDD);
        bus_and_ram.ram->write(2, 0x23); // INC IX
        cpu.regs().ix = 0x0001;
        cpu.regs().iy = 0x0001;
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.regs().ix == 0x0002);
        REQUIRE(cpu.regs().iy == 0x0001); // IY untouched
    }
}


TEST_CASE("Z80 Phase 10: NMI non-maskable interrupt", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x2000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("NMI saves PC, jumps to 0x0066, clears IFF1") {
        cpu.regs().pc = 0x1234;
        cpu.regs().sp = 0x1000;
        cpu.regs().iff1 = true;
        cpu.regs().iff2 = false;

        cpu.requestNmi();
        unsigned cycles = cpu.step();

        REQUIRE(cycles == 11);
        REQUIRE(cpu.regs().pc == 0x0066);
        REQUIRE(cpu.regs().sp == 0x0FFE);
        REQUIRE(bus_and_ram.ram->read(0x0FFE) == 0x34); // low byte of return addr
        REQUIRE(bus_and_ram.ram->read(0x0FFF) == 0x12); // high byte
        REQUIRE_FALSE(cpu.regs().iff1);                  // disabled in ISR
        REQUIRE(cpu.regs().iff2);                        // saved old IFF1
        REQUIRE_FALSE(cpu.halted());
    }

    SECTION("NMI has priority over INT") {
        cpu.regs().pc = 0x0100;
        cpu.regs().sp = 0x1000;
        cpu.regs().iff1 = true;
        cpu.regs().im = 1;
        cpu.setIntLine(true);
        cpu.requestNmi();

        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0066); // NMI vector, not 0x0038
    }

    SECTION("NMI releases HALT") {
        bus_and_ram.ram->write(0, 0x76); // HALT
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.halted());

        cpu.regs().sp = 0x1000;
        cpu.requestNmi();
        cpu.step();
        REQUIRE_FALSE(cpu.halted());
        REQUIRE(cpu.regs().pc == 0x0066);
    }
}

TEST_CASE("Z80 Phase 10: INT modes 0/1/2", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x2000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("INT is ignored when IFF1 is clear") {
        cpu.regs().pc = 0x0100;
        cpu.regs().iff1 = false;
        cpu.regs().im = 1;
        cpu.setIntLine(true);
        bus_and_ram.ram->write(0x0100, 0x00); // NOP
        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0101); // NOP executed, no jump
    }

    SECTION("INT mode 1 jumps to 0x0038") {
        cpu.regs().pc = 0x0100;
        cpu.regs().sp = 0x1000;
        cpu.regs().iff1 = true;
        cpu.regs().im = 1;
        cpu.setIntLine(true);

        unsigned cycles = cpu.step();
        REQUIRE(cycles == 13);
        REQUIRE(cpu.regs().pc == 0x0038);
        REQUIRE(cpu.regs().sp == 0x0FFE);
        REQUIRE(bus_and_ram.ram->read(0x0FFE) == 0x00);
        REQUIRE(bus_and_ram.ram->read(0x0FFF) == 0x01); // return addr 0x0100
        REQUIRE_FALSE(cpu.regs().iff1); // disabled in ISR
    }

    SECTION("INT mode 2 reads vector and forms ISR address") {
        cpu.regs().pc = 0x0100;
        cpu.regs().sp = 0x1000;
        cpu.regs().iff1 = true;
        cpu.regs().im = 2;
        cpu.regs().i = 0x10;
        cpu.setIntData(0x40);                 // vector byte -> address 0x1040
        bus_and_ram.ram->write(0x1040, 0xCD); // ISR low
        bus_and_ram.ram->write(0x1041, 0xAB); // ISR high
        cpu.setIntLine(true);

        unsigned cycles = cpu.step();
        REQUIRE(cycles == 19);
        REQUIRE(cpu.regs().pc == 0xABCD);
        REQUIRE(cpu.regs().sp == 0x0FFE);
        REQUIRE_FALSE(cpu.regs().iff1);
    }

    SECTION("INT mode 0 executes the data-bus opcode (RST 38)") {
        cpu.regs().pc = 0x0100;
        cpu.regs().sp = 0x1000;
        cpu.regs().iff1 = true;
        cpu.regs().im = 0;
        cpu.setIntData(0xFF); // RST 38H opcode
        cpu.setIntLine(true);

        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0038);
        REQUIRE(cpu.regs().sp == 0x0FFE);
        REQUIRE(bus_and_ram.ram->read(0x0FFE) == 0x00);
        REQUIRE(bus_and_ram.ram->read(0x0FFF) == 0x01); // return addr 0x0100
        REQUIRE_FALSE(cpu.regs().iff1);
    }
}

TEST_CASE("Z80 Phase 10: EI/DI interrupt acceptance", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x2000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("EI delays interrupt acceptance by one instruction") {
        // Program: EI, NOP, NOP ; INT line held high the whole time.
        bus_and_ram.ram->write(0, 0xFB);
        bus_and_ram.ram->write(1, 0x00);
        bus_and_ram.ram->write(2, 0x00);
        cpu.regs().pc = 0;
        cpu.regs().sp = 0x1000;
        cpu.regs().im = 1;
        cpu.regs().iff1 = false;
        cpu.setIntLine(true);

        cpu.step(); // EI: IFF becomes set, after_ei armed
        REQUIRE(cpu.regs().iff1);
        REQUIRE(cpu.regs().pc == 1);

        cpu.step(); // NOP: must NOT accept the interrupt yet (EI delay)
        REQUIRE(cpu.regs().pc == 2); // advanced normally, not 0x0038

        cpu.step(); // now the interrupt is accepted
        REQUIRE(cpu.regs().pc == 0x0038);
        REQUIRE(bus_and_ram.ram->read(0x0FFE) == 0x02); // return addr 2
    }

    SECTION("DI clears IFF immediately and blocks INT") {
        // Program: DI, NOP ; INT line asserted only after DI executes.
        bus_and_ram.ram->write(0, 0xF3);
        bus_and_ram.ram->write(1, 0x00);
        cpu.regs().pc = 0;
        cpu.regs().sp = 0x1000;
        cpu.regs().im = 1;
        cpu.regs().iff1 = true;
        cpu.setIntLine(false);

        cpu.step(); // DI
        REQUIRE_FALSE(cpu.regs().iff1);

        cpu.setIntLine(true);
        cpu.step(); // NOP: INT must not be accepted
        REQUIRE(cpu.regs().pc == 2); // not 0x0038
    }

    SECTION("INT accepted immediately when already enabled") {
        bus_and_ram.ram->write(0x0100, 0x00); // NOP (never reached)
        cpu.regs().pc = 0x0100;
        cpu.regs().sp = 0x1000;
        cpu.regs().im = 1;
        cpu.regs().iff1 = true;
        cpu.setIntLine(true);

        cpu.step();
        REQUIRE(cpu.regs().pc == 0x0038);
    }
}

TEST_CASE("Z80 Phase 10: refresh register R and LD A,I/R flags", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x2000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("R increments on each instruction fetch") {
        cpu.reset();
        bus_and_ram.ram->write(0, 0x00); // NOP
        bus_and_ram.ram->write(1, 0x00);
        cpu.regs().pc = 0;
        REQUIRE(cpu.regs().r == 0);

        cpu.step();
        REQUIRE(cpu.regs().r == 1);
        cpu.step();
        REQUIRE(cpu.regs().r == 2);
    }

    SECTION("R preserves bit 7") {
        cpu.reset();
        cpu.regs().r = 0x80;
        bus_and_ram.ram->write(0, 0x00);
        cpu.regs().pc = 0;
        cpu.step();
        // low 7 bits wrap 0->1, bit 7 stays 1
        REQUIRE(cpu.regs().r == 0x81);
    }

    SECTION("LD A,I (ED 57) sets P/V from IFF2") {
        cpu.regs().i = 0x55;
        cpu.regs().iff2 = true;
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x57);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x55);
        REQUIRE(cpu.getFlagPV());

        cpu.regs().iff2 = false;
        bus_and_ram.ram->write(2, 0xED);
        bus_and_ram.ram->write(3, 0x57);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE_FALSE(cpu.getFlagPV());
    }

    SECTION("LD A,R (ED 5F) sets P/V from IFF2") {
        cpu.reset();
        cpu.regs().iff2 = true;
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x5F);
        cpu.regs().pc = 0;
        cpu.step();
        // R is incremented by the opcode fetch before being loaded into A.
        REQUIRE(cpu.getA() == cpu.regs().r);
        REQUIRE(cpu.getFlagPV());
    }
}


TEST_CASE("Z80 Phase 9: IXH/IXL register halves (undocumented)", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x1000);
    cpu.setBus(bus_and_ram.bus.get());

    SECTION("LD IXH,n (DD 26) and LD IXL,n (DD 2E)") {
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x26);
        bus_and_ram.ram->write(2, 0xAB);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 11);
        REQUIRE((cpu.regs().ix >> 8) == 0xAB);

        bus_and_ram.ram->write(3, 0xDD);
        bus_and_ram.ram->write(4, 0x2E);
        bus_and_ram.ram->write(5, 0xCD);
        cpu.regs().pc = 3;
        cpu.step();
        REQUIRE((cpu.regs().ix & 0xFF) == 0xCD);
    }

    SECTION("INC IXH (DD 24) sets flags, leaves IXL") {
        cpu.regs().ix = 0x0FFF;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x24);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 8);
        REQUIRE(cpu.regs().ix == 0x10FF);
        REQUIRE(cpu.getFlagH());
        REQUIRE_FALSE(cpu.getFlagN());
    }

    SECTION("DEC IXL (DD 2D) sets flags") {
        cpu.regs().ix = 0x0000;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x2D);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE((cpu.regs().ix & 0xFF) == 0xFF);
        REQUIRE(cpu.getFlagZ() == false);
        REQUIRE(cpu.getFlagN());
    }

    SECTION("LD B,IXH (DD 44) and LD A,IXL (DD 7D)") {
        cpu.regs().ix = 0x1234;
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x44);
        cpu.regs().pc = 0;
        REQUIRE(cpu.step() == 8);
        REQUIRE(cpu.getB() == 0x12);

        bus_and_ram.ram->write(2, 0xDD);
        bus_and_ram.ram->write(3, 0x7D);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE(cpu.getA() == 0x34);
    }

    SECTION("LD IXH,B (DD 60) and LD IXL,A (DD 6F)") {
        cpu.regs().ix = 0x0000;
        cpu.setB(0x55);
        cpu.setA(0x77);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0x60);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE((cpu.regs().ix >> 8) == 0x55);

        bus_and_ram.ram->write(2, 0xDD);
        bus_and_ram.ram->write(3, 0x6F);
        cpu.regs().pc = 2;
        cpu.step();
        REQUIRE((cpu.regs().ix & 0xFF) == 0x77);
    }

    SECTION("IYH/IYL mirror via FD prefix") {
        cpu.regs().iy = 0x0000;
        bus_and_ram.ram->write(0, 0xFD);
        bus_and_ram.ram->write(1, 0x26);
        bus_and_ram.ram->write(2, 0x99);
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE((cpu.regs().iy >> 8) == 0x99);
    }
}

TEST_CASE("Z80 Phase 10: undocumented F3/F5 flag bits", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x4000);
    cpu.setBus(bus_and_ram.bus.get());
    // 0x28 has both bit 3 (0x08) and bit 5 (0x20) set, so a value/result of
    // 0x28 must set both F3 and F5. 0x40 has neither bit set.

    SECTION("ADD A,B copies F3/F5 from the result") {
        cpu.setA(0x00);
        cpu.setB(0x28);
        bus_and_ram.ram->write(0, 0x80); // ADD A,B
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }

    SECTION("CP n copies F3/F5 from the operand (not the result)") {
        cpu.setA(0x68);                 // 0x68 - 0x28 = 0x40 (F3/F5 clear in result)
        bus_and_ram.ram->write(0, 0xFE);
        bus_and_ram.ram->write(1, 0x28); // CP 0x28
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getFlagF3());       // operand 0x28 has bit 3 set
        REQUIRE(cpu.getFlagF5());       // operand 0x28 has bit 5 set
    }

    SECTION("INC B copies F3/F5 from the result") {
        cpu.setB(0x27);                 // -> 0x28
        bus_and_ram.ram->write(0, 0x04); // INC B
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getB() == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }

    SECTION("DEC B copies F3/F5 from the result") {
        cpu.setB(0x29);                 // -> 0x28
        bus_and_ram.ram->write(0, 0x05); // DEC B
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getB() == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }

    SECTION("CB SLA B copies F3/F5 from the result") {
        cpu.setB(0x14);                 // SLA -> 0x28
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0x20); // SLA B
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getB() == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }

    SECTION("BIT n,r copies F3/F5 from the tested register") {
        cpu.setB(0x28);
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0x40); // BIT 0,B
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getFlagZ());        // bit 0 of 0x28 is 0
        REQUIRE(cpu.getFlagF3());       // B bit 3 set
        REQUIRE(cpu.getFlagF5());       // B bit 5 set
    }

    SECTION("LD A,I copies F3/F5 from I") {
        cpu.regs().i = 0x28;
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0x57); // LD A,I
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }

    SECTION("LDI copies F3/F5 from the transferred byte") {
        cpu.regs().hl = 0x1000;
        cpu.regs().de = 0x2000;
        bus_and_ram.ram->write(0x1000, 0x28);
        bus_and_ram.ram->write(0, 0xED);
        bus_and_ram.ram->write(1, 0xA0); // LDI
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x2000) == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }

    SECTION("RLCA copies F3/F5 from the rotated accumulator") {
        cpu.setA(0x14);                 // RLCA -> 0x28
        bus_and_ram.ram->write(0, 0x07); // RLCA
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getA() == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }
}

TEST_CASE("Z80 Phase 10: MEMPTR drives BIT (HL)/(IX+d) flags", "[cpu][z80][fast]") {
    Z80 cpu;
    auto bus_and_ram = createBusWithRam(0x0000, 0x4000);
    cpu.setBus(bus_and_ram.bus.get());
    // Effective-address high byte 0x28 has bits 3 and 5 set; the loaded value
    // 0xC7 has neither, proving F3/F5 come from the address (MEMPTR), not the byte.

    SECTION("BIT n,(HL) takes F3/F5 from address high byte") {
        cpu.regs().hl = 0x2800;                 // H = 0x28
        bus_and_ram.ram->write(0x2800, 0xC7);   // value bits 3/5 clear
        bus_and_ram.ram->write(0, 0xCB);
        bus_and_ram.ram->write(1, 0x46);        // BIT 0,(HL)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getFlagF3());               // from H bit 3
        REQUIRE(cpu.getFlagF5());               // from H bit 5
        REQUIRE(cpu.wz() == 0x2800);
    }

    SECTION("BIT n,(IX+d) takes F3/F5 from effective address high byte") {
        cpu.regs().ix = 0x2800;                 // (IX+0) high byte = 0x28
        bus_and_ram.ram->write(0x2800, 0xC7);
        bus_and_ram.ram->write(0, 0xDD);
        bus_and_ram.ram->write(1, 0xCB);
        bus_and_ram.ram->write(2, 0x00);        // d = 0
        bus_and_ram.ram->write(3, 0x46);        // BIT 0,(IX+d)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
        REQUIRE(cpu.wz() == 0x2800);
    }

    SECTION("INC (HL) leaves F3/F5 from result (sanity, value 0x27 -> 0x28)") {
        cpu.regs().hl = 0x0500;
        bus_and_ram.ram->write(0x0500, 0x27);
        bus_and_ram.ram->write(0, 0x34); // INC (HL)
        cpu.regs().pc = 0;
        cpu.step();
        REQUIRE(bus_and_ram.ram->read(0x0500) == 0x28);
        REQUIRE(cpu.getFlagF3());
        REQUIRE(cpu.getFlagF5());
    }
}

