#include "cpus/z80.hpp"
#include <algorithm>

// Phase 3 Base loads and stack (opcodes 00–3F)

// 0x00 - 0x07
unsigned Z80::op00() { // NOP
    return 4;
}

unsigned Z80::op01() { // LD BC,nn
    regs_.bc = fetchWord();
    return 10;
}

unsigned Z80::op02() { // LD (BC),A
    writeByte(regs_.bc, getA());
    return 7;
}

unsigned Z80::op03() { // INC BC
    regs_.bc++;
    return 6;
}

unsigned Z80::op04() { setB(inc8(getB())); return 4; } // INC B

unsigned Z80::op05() { setB(dec8(getB())); return 4; } // DEC B

unsigned Z80::op06() { // LD B,n
    setB(fetchByte());
    return 7;
}

unsigned Z80::op07() { // RLCA
    uint8_t a = getA();
    uint8_t carry = (a & 0x80) >> 7;
    a = (a << 1) | carry;
    setA(a);
    setFlagC(carry != 0);
    setFlagH(false);
    setFlagN(false);
    setF35(a);
    return 4;
}

// 0x08 - 0x0F
unsigned Z80::op08() { // EX AF,AF'
    std::swap(regs_.af, regs_.af_);
    return 4;
}

unsigned Z80::op09() { // ADD HL,BC
    add16(regs_.hl, regs_.bc);
    return 11;
}

unsigned Z80::op0A() { // LD A,(BC)
    setA(readByte(regs_.bc));
    return 7;
}

unsigned Z80::op0B() { // DEC BC
    regs_.bc--;
    return 6;
}

unsigned Z80::op0C() { setC(inc8(getC())); return 4; } // INC C

unsigned Z80::op0D() { setC(dec8(getC())); return 4; } // DEC C

unsigned Z80::op0E() { // LD C,n
    setC(fetchByte());
    return 7;
}

unsigned Z80::op0F() { // RRCA
    uint8_t a = getA();
    uint8_t carry = a & 0x01;
    a = (a >> 1) | (carry << 7);
    setA(a);
    setFlagC(carry != 0);
    setFlagH(false);
    setFlagN(false);
    setF35(a);
    return 4;
}

// 0x10 - 0x17
unsigned Z80::op10() { // DJNZ e
    int8_t disp = static_cast<int8_t>(fetchByte());
    uint8_t b = getB() - 1;
    setB(b);
    if (b != 0) {
        regs_.pc = static_cast<uint16_t>(regs_.pc + disp);
        return 13;
    }
    return 8;
}

unsigned Z80::op11() { // LD DE,nn
    regs_.de = fetchWord();
    return 10;
}

unsigned Z80::op12() { // LD (DE),A
    writeByte(regs_.de, getA());
    return 7;
}

unsigned Z80::op13() { // INC DE
    regs_.de++;
    return 6;
}

unsigned Z80::op14() { setD(inc8(getD())); return 4; } // INC D

unsigned Z80::op15() { setD(dec8(getD())); return 4; } // DEC D

unsigned Z80::op16() { // LD D,n
    setD(fetchByte());
    return 7;
}

unsigned Z80::op17() { // RLA
    uint8_t a = getA();
    uint8_t old_c = getFlagC() ? 1 : 0;
    uint8_t new_c = (a & 0x80) >> 7;
    a = (a << 1) | old_c;
    setA(a);
    setFlagC(new_c != 0);
    setFlagH(false);
    setFlagN(false);
    setF35(a);
    return 4;
}

// 0x18 - 0x1F
unsigned Z80::op18() { // JR e
    int8_t disp = static_cast<int8_t>(fetchByte());
    regs_.pc = static_cast<uint16_t>(regs_.pc + disp);
    return 12;
}

unsigned Z80::op19() { // ADD HL,DE
    add16(regs_.hl, regs_.de);
    return 11;
}

unsigned Z80::op1A() { // LD A,(DE)
    setA(readByte(regs_.de));
    return 7;
}

unsigned Z80::op1B() { // DEC DE
    regs_.de--;
    return 6;
}

unsigned Z80::op1C() { setE(inc8(getE())); return 4; } // INC E

unsigned Z80::op1D() { setE(dec8(getE())); return 4; } // DEC E

unsigned Z80::op1E() { // LD E,n
    setE(fetchByte());
    return 7;
}

unsigned Z80::op1F() { // RRA
    uint8_t a = getA();
    uint8_t old_c = getFlagC() ? 1 : 0;
    uint8_t new_c = a & 0x01;
    a = (a >> 1) | (old_c << 7);
    setA(a);
    setFlagC(new_c != 0);
    setFlagH(false);
    setFlagN(false);
    setF35(a);
    return 4;
}

// 0x20 - 0x27
unsigned Z80::op20() { // JR NZ,e
    int8_t disp = static_cast<int8_t>(fetchByte());
    if (!getFlagZ()) {
        regs_.pc = static_cast<uint16_t>(regs_.pc + disp);
        return 12;
    }
    return 7;
}

unsigned Z80::op21() { // LD HL,nn
    regs_.hl = fetchWord();
    return 10;
}

unsigned Z80::op22() { // LD (nn),HL
    uint16_t addr = fetchWord();
    writeByte(addr, regs_.hl & 0xFF);
    writeByte(addr + 1, regs_.hl >> 8);
    return 16;
}

unsigned Z80::op23() { // INC HL
    regs_.hl++;
    return 6;
}

unsigned Z80::op24() { setH(inc8(getH())); return 4; } // INC H

unsigned Z80::op25() { setH(dec8(getH())); return 4; } // DEC H

unsigned Z80::op26() { // LD H,n
    setH(fetchByte());
    return 7;
}

unsigned Z80::op27() { // DAA
    daa();
    return 4;
}

// 0x28 - 0x2F
unsigned Z80::op28() { // JR Z,e
    int8_t disp = static_cast<int8_t>(fetchByte());
    if (getFlagZ()) {
        regs_.pc = static_cast<uint16_t>(regs_.pc + disp);
        return 12;
    }
    return 7;
}

unsigned Z80::op29() { // ADD HL,HL
    add16(regs_.hl, regs_.hl);
    return 11;
}

unsigned Z80::op2A() { // LD HL,(nn)
    uint16_t addr = fetchWord();
    uint8_t l = readByte(addr);
    uint8_t h = readByte(addr + 1);
    regs_.hl = (h << 8) | l;
    return 16;
}

unsigned Z80::op2B() { // DEC HL
    regs_.hl--;
    return 6;
}

unsigned Z80::op2C() { setL(inc8(getL())); return 4; } // INC L

unsigned Z80::op2D() { setL(dec8(getL())); return 4; } // DEC L

unsigned Z80::op2E() { // LD L,n
    setL(fetchByte());
    return 7;
}

unsigned Z80::op2F() { // CPL
    setA(~getA());
    setFlagH(true);
    setFlagN(true);
    return 4;
}

// 0x30 - 0x37
unsigned Z80::op30() { // JR NC,e
    int8_t disp = static_cast<int8_t>(fetchByte());
    if (!getFlagC()) {
        regs_.pc = static_cast<uint16_t>(regs_.pc + disp);
        return 12;
    }
    return 7;
}

unsigned Z80::op31() { // LD SP,nn
    regs_.sp = fetchWord();
    return 10;
}

unsigned Z80::op32() { // LD (nn),A
    writeByte(fetchWord(), getA());
    return 13;
}

unsigned Z80::op33() { // INC SP
    regs_.sp++;
    return 6;
}

unsigned Z80::op34() { // INC (HL)
    uint8_t res = inc8(readByte(regs_.hl));
    writeByte(regs_.hl, res);
    return 11;
}

unsigned Z80::op35() { // DEC (HL)
    uint8_t res = dec8(readByte(regs_.hl));
    writeByte(regs_.hl, res);
    return 11;
}

unsigned Z80::op36() { // LD (HL),n
    writeByte(regs_.hl, fetchByte());
    return 10;
}

unsigned Z80::op37() { // SCF
    setFlagC(true);
    setFlagH(false);
    setFlagN(false);
    return 4;
}

// 0x38 - 0x3F
unsigned Z80::op38() { // JR C,e
    int8_t disp = static_cast<int8_t>(fetchByte());
    if (getFlagC()) {
        regs_.pc = static_cast<uint16_t>(regs_.pc + disp);
        return 12;
    }
    return 7;
}

unsigned Z80::op39() { // ADD HL,SP
    add16(regs_.hl, regs_.sp);
    return 11;
}

unsigned Z80::op3A() { // LD A,(nn)
    setA(readByte(fetchWord()));
    return 13;
}

unsigned Z80::op3B() { // DEC SP
    regs_.sp--;
    return 6;
}

unsigned Z80::op3C() { setA(inc8(getA())); return 4; } // INC A

unsigned Z80::op3D() { setA(dec8(getA())); return 4; } // DEC A

unsigned Z80::op3E() { // LD A,n
    setA(fetchByte());
    return 7;
}

unsigned Z80::op3F() { // CCF
    bool old_c = getFlagC();
    setFlagC(!old_c);
    setFlagH(old_c);
    setFlagN(false);
    return 4;
}
