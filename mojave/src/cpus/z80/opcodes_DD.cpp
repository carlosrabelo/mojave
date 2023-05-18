#include "cpus/z80.hpp"
#include "cpus/z80/dispatch.hpp"

// Phase 9 DD/FD prefix: IX/IY indexed addressing, DDCB/FDCB compound ops.
// DD selects IX, FD selects IY. The active prefix flag (prefix_dd_/prefix_fd_)
// is set by opDD_prefix()/opFD_prefix() before executeDD() runs. getReg/setReg
// route the H/L/(HL) slots to IXH/IXL/(IX+d) while a flag is set.
//
// Z80 quirk: when one operand of LD r,r' is (IX+d) (slot 6), the other operand
// refers to the REAL H/L register (not IXH/IXL). This is handled by clearing the
// prefix flag for the indexed LD path so register slots resolve to B,C,D,E,H,L,A.

uint16_t& Z80::indexReg() {
    return prefix_dd_ ? regs_.ix : regs_.iy;
}

void Z80::incIndexHalf(int idx) { setReg(idx, inc8(getReg(idx))); } // INC IXH/IXL

void Z80::decIndexHalf(int idx) { setReg(idx, dec8(getReg(idx))); } // DEC IXH/IXL

void Z80::incIndexedMem() { // INC (IX+d)
    int8_t d = fetchDisp();
    uint16_t addr = indexReg() + d;
    wz_ = addr;
    uint8_t res = inc8(readByte(addr));
    writeByte(addr, res);
}

void Z80::decIndexedMem() { // DEC (IX+d)
    int8_t d = fetchDisp();
    uint16_t addr = indexReg() + d;
    wz_ = addr;
    uint8_t res = dec8(readByte(addr));
    writeByte(addr, res);
}

unsigned Z80::executeDD(uint8_t op) {
    switch (op) {
        // 16-bit index register operations
        case 0x09: { add16(indexReg(), regs_.bc); return 15; }           // ADD IX,BC
        case 0x19: { add16(indexReg(), regs_.de); return 15; }           // ADD IX,DE
        case 0x21: indexReg() = fetchWord(); return 14;                  // LD IX,nn
        case 0x22: {                                                     // LD (nn),IX
            uint16_t addr = fetchWord();
            uint16_t v = indexReg();
            writeByte(addr, v & 0xFF);
            writeByte(addr + 1, v >> 8);
            return 20;
        }
        case 0x23: indexReg()++; return 10;                              // INC IX
        case 0x24: incIndexHalf(4); return 8;                            // INC IXH*
        case 0x25: decIndexHalf(4); return 8;                            // DEC IXH*
        case 0x26: setReg(4, fetchByte()); return 11;                    // LD IXH,n*
        case 0x29: { uint16_t v = indexReg(); add16(indexReg(), v); return 15; } // ADD IX,IX
        case 0x2A: {                                                     // LD IX,(nn)
            uint16_t addr = fetchWord();
            uint8_t lo = readByte(addr);
            uint8_t hi = readByte(addr + 1);
            indexReg() = (hi << 8) | lo;
            return 20;
        }
        case 0x2B: indexReg()--; return 10;                              // DEC IX
        case 0x2C: incIndexHalf(5); return 8;                            // INC IXL*
        case 0x2D: decIndexHalf(5); return 8;                            // DEC IXL*
        case 0x2E: setReg(5, fetchByte()); return 11;                    // LD IXL,n*
        case 0x34: incIndexedMem(); return 23;                           // INC (IX+d)
        case 0x35: decIndexedMem(); return 23;                           // DEC (IX+d)
        case 0x36: {                                                     // LD (IX+d),n
            int8_t d = fetchDisp();
            uint16_t addr = indexReg() + d;
            writeByte(addr, fetchByte());
            return 19;
        }
        case 0x39: { add16(indexReg(), regs_.sp); return 15; }           // ADD IX,SP

        // Compound and nested prefixes
        case 0xCB: return executeDDCB();
        case 0xDD: return opDD_prefix();   // last prefix wins
        case 0xFD: return opFD_prefix();

        // Stack and control flow using the index register
        case 0xE1: indexReg() = pop16(); return 14;                      // POP IX
        case 0xE3: {                                                     // EX (SP),IX
            uint16_t sp = regs_.sp;
            uint8_t lo = readByte(sp);
            uint8_t hi = readByte(sp + 1);
            uint16_t spval = (hi << 8) | lo;
            uint16_t v = indexReg();
            writeByte(sp, v & 0xFF);
            writeByte(sp + 1, v >> 8);
            indexReg() = spval;
            return 23;
        }
        case 0xE5: push16(indexReg()); return 15;                        // PUSH IX
        case 0xE9: regs_.pc = indexReg(); return 8;                      // JP (IX)
        case 0xF9: regs_.sp = indexReg(); return 10;                     // LD SP,IX
        default: break;
    }

    // LD r,r' block (0x40-0x7F): H/L slots become IXH/IXL, (HL) becomes (IX+d).
    if (op >= 0x40 && op <= 0x7F) {
        if (op == 0x76) { is_halted = true; return 4; }                  // HALT
        int dest = (op >> 3) & 7;
        int src = op & 7;
        if (dest == 6 || src == 6) {
            // Indexed: one operand is (IX+d); the other is a real register
            // (H/L resolve to real H/L, not IXH/IXL).
            int8_t d = fetchDisp();
            uint16_t addr = indexReg() + d;
            prefix_dd_ = false;
            prefix_fd_ = false;
            if (dest == 6) {
                writeByte(addr, getReg(src));
            } else {
                setReg(dest, readByte(addr));
            }
            return 19;
        }
        setReg(dest, getReg(src));
        return 8;
    }

    // 8-bit ALU block (0x80-0xBF): source slot routed via flag.
    if (op >= 0x80 && op <= 0xBF) {
        int kind = (op >> 3) & 7;
        int src = op & 7;
        uint8_t val = getReg(src);
        switch (kind) {
            case 0: aluADD(val); break;
            case 1: aluADC(val); break;
            case 2: aluSUB(val); break;
            case 3: aluSBC(val); break;
            case 4: aluAND(val); break;
            case 5: aluXOR(val); break;
            case 6: aluOR(val); break;
            case 7: aluCP(val); break;
        }
        return (src == 6) ? 19 : 8;
    }

    // Fallthrough: DD/FD acts as a NOP (4 T) and the base opcode runs unprefixed.
    prefix_dd_ = false;
    prefix_fd_ = false;
    return 4 + (this->*z80::kDispatch[op])();
}

unsigned Z80::executeDDCB() {
    int8_t d = fetchDisp();
    uint8_t op = fetchByte();
    uint16_t addr = indexReg() + d;
    // Register copy targets the real B,C,D,E,H,L,A (not IXH/IXL).
    prefix_dd_ = false;
    prefix_fd_ = false;

    int type = (op >> 6) & 3;
    int b = (op >> 3) & 7;
    int r = op & 7;
    uint8_t val = readByte(addr);

    if (type == 0) { // Shift/Rotate
        uint8_t res = cbShift(b, val);
        writeByte(addr, res);
        setFlagS((res & 0x80) != 0);
        setFlagZ(res == 0);
        setFlagH(false);
        setFlagPV(parity(res));
        setFlagN(false);
        setF35(res);
        if (r != 6) setReg(r, res);
        return 23;
    }
    if (type == 1) { // BIT
        bool bit_val = (val & (1 << b)) != 0;
        setFlagZ(!bit_val);
        setFlagH(true);
        setFlagN(false);
        if (b == 7) setFlagS(bit_val);
        setFlagPV(!bit_val);
        // F3/F5 are driven by the high byte of the effective address (WZ high).
        wz_ = addr;
        setF35(static_cast<uint8_t>(wz_ >> 8));
        return 20;
    }
    // type 2 = RES, type 3 = SET
    uint8_t res = (type == 2) ? static_cast<uint8_t>(val & ~(1 << b))
                              : static_cast<uint8_t>(val | (1 << b));
    writeByte(addr, res);
    if (r != 6) setReg(r, res);
    return 23;
}
