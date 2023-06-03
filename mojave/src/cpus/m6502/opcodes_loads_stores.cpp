#include "cpus/m6502.hpp"

// --- LOADS ---

// LDA
unsigned M6502::opA9() { // LDA #imm
    uint16_t addr = addrImmediate();
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return 2;
}

unsigned M6502::opA5() { // LDA zp
    uint16_t addr = addrZeroPage();
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return 3;
}

unsigned M6502::opB5() { // LDA zp,X
    uint16_t addr = addrZeroPageX();
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return 4;
}

unsigned M6502::opAD() { // LDA abs
    uint16_t addr = addrAbsolute();
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return 4;
}

unsigned M6502::opBD() { // LDA abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return page_crossed ? 5 : 4;
}

unsigned M6502::opB9() { // LDA abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return page_crossed ? 5 : 4;
}

unsigned M6502::opA1() { // LDA (zp,X)
    uint16_t addr = addrIndexedIndirect();
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return 6;
}

unsigned M6502::opB1() { // LDA (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    regs_.a = readByte(addr);
    updateNZ(regs_.a);
    return page_crossed ? 6 : 5;
}

// LDX
unsigned M6502::opA2() { // LDX #imm
    uint16_t addr = addrImmediate();
    regs_.x = readByte(addr);
    updateNZ(regs_.x);
    return 2;
}

unsigned M6502::opA6() { // LDX zp
    uint16_t addr = addrZeroPage();
    regs_.x = readByte(addr);
    updateNZ(regs_.x);
    return 3;
}

unsigned M6502::opB6() { // LDX zp,Y
    uint16_t addr = addrZeroPageY();
    regs_.x = readByte(addr);
    updateNZ(regs_.x);
    return 4;
}

unsigned M6502::opAE() { // LDX abs
    uint16_t addr = addrAbsolute();
    regs_.x = readByte(addr);
    updateNZ(regs_.x);
    return 4;
}

unsigned M6502::opBE() { // LDX abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    regs_.x = readByte(addr);
    updateNZ(regs_.x);
    return page_crossed ? 5 : 4;
}

// LDY
unsigned M6502::opA0() { // LDY #imm
    uint16_t addr = addrImmediate();
    regs_.y = readByte(addr);
    updateNZ(regs_.y);
    return 2;
}

unsigned M6502::opA4() { // LDY zp
    uint16_t addr = addrZeroPage();
    regs_.y = readByte(addr);
    updateNZ(regs_.y);
    return 3;
}

unsigned M6502::opB4() { // LDY zp,X
    uint16_t addr = addrZeroPageX();
    regs_.y = readByte(addr);
    updateNZ(regs_.y);
    return 4;
}

unsigned M6502::opAC() { // LDY abs
    uint16_t addr = addrAbsolute();
    regs_.y = readByte(addr);
    updateNZ(regs_.y);
    return 4;
}

unsigned M6502::opBC() { // LDY abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    regs_.y = readByte(addr);
    updateNZ(regs_.y);
    return page_crossed ? 5 : 4;
}

// --- STORES ---

// STA
unsigned M6502::op85() { // STA zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, regs_.a);
    return 3;
}

unsigned M6502::op95() { // STA zp,X
    uint16_t addr = addrZeroPageX();
    writeByte(addr, regs_.a);
    return 4;
}

unsigned M6502::op8D() { // STA abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, regs_.a);
    return 4;
}

unsigned M6502::op9D() { // STA abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    writeByte(addr, regs_.a);
    return 5;
}

unsigned M6502::op99() { // STA abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    writeByte(addr, regs_.a);
    return 5;
}

unsigned M6502::op81() { // STA (zp,X)
    uint16_t addr = addrIndexedIndirect();
    writeByte(addr, regs_.a);
    return 6;
}

unsigned M6502::op91() { // STA (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    writeByte(addr, regs_.a);
    return 6;
}

// STX
unsigned M6502::op86() { // STX zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, regs_.x);
    return 3;
}

unsigned M6502::op96() { // STX zp,Y
    uint16_t addr = addrZeroPageY();
    writeByte(addr, regs_.x);
    return 4;
}

unsigned M6502::op8E() { // STX abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, regs_.x);
    return 4;
}

// STY
unsigned M6502::op84() { // STY zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, regs_.y);
    return 3;
}

unsigned M6502::op94() { // STY zp,X
    uint16_t addr = addrZeroPageX();
    writeByte(addr, regs_.y);
    return 4;
}

unsigned M6502::op8C() { // STY abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, regs_.y);
    return 4;
}

// --- TRANSFERS ---

unsigned M6502::opAA() { // TAX
    regs_.x = regs_.a;
    updateNZ(regs_.x);
    return 2;
}

unsigned M6502::opA8() { // TAY
    regs_.y = regs_.a;
    updateNZ(regs_.y);
    return 2;
}

unsigned M6502::op8A() { // TXA
    regs_.a = regs_.x;
    updateNZ(regs_.a);
    return 2;
}

unsigned M6502::op98() { // TYA
    regs_.a = regs_.y;
    updateNZ(regs_.a);
    return 2;
}

unsigned M6502::opBA() { // TSX
    regs_.x = regs_.sp;
    updateNZ(regs_.x);
    return 2;
}

unsigned M6502::op9A() { // TXS
    regs_.sp = regs_.x;
    return 2;
}

// --- STACK ---

unsigned M6502::op48() { // PHA
    push8(regs_.a);
    return 3;
}

unsigned M6502::op68() { // PLA
    regs_.a = pop8();
    updateNZ(regs_.a);
    return 4;
}

unsigned M6502::op08() { // PHP
    push8(regs_.p | 0x30);
    return 3;
}

unsigned M6502::op28() { // PLP
    regs_.p = (pop8() & ~0x10) | 0x20;
    return 4;
}
