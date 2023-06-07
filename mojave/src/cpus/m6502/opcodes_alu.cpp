#include "cpus/m6502.hpp"

// --- AND ---

unsigned M6502::op29() { // AND #imm
    uint16_t addr = addrImmediate();
    aluAND(readByte(addr));
    return 2;
}

unsigned M6502::op25() { // AND zp
    uint16_t addr = addrZeroPage();
    aluAND(readByte(addr));
    return 3;
}

unsigned M6502::op35() { // AND zp,X
    uint16_t addr = addrZeroPageX();
    aluAND(readByte(addr));
    return 4;
}

unsigned M6502::op2D() { // AND abs
    uint16_t addr = addrAbsolute();
    aluAND(readByte(addr));
    return 4;
}

unsigned M6502::op3D() { // AND abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    aluAND(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op39() { // AND abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    aluAND(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op21() { // AND (zp,X)
    uint16_t addr = addrIndexedIndirect();
    aluAND(readByte(addr));
    return 6;
}

unsigned M6502::op31() { // AND (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    aluAND(readByte(addr));
    return page_crossed ? 6 : 5;
}

// --- ORA ---

unsigned M6502::op09() { // ORA #imm
    uint16_t addr = addrImmediate();
    aluORA(readByte(addr));
    return 2;
}

unsigned M6502::op05() { // ORA zp
    uint16_t addr = addrZeroPage();
    aluORA(readByte(addr));
    return 3;
}

unsigned M6502::op15() { // ORA zp,X
    uint16_t addr = addrZeroPageX();
    aluORA(readByte(addr));
    return 4;
}

unsigned M6502::op0D() { // ORA abs
    uint16_t addr = addrAbsolute();
    aluORA(readByte(addr));
    return 4;
}

unsigned M6502::op1D() { // ORA abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    aluORA(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op19() { // ORA abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    aluORA(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op01() { // ORA (zp,X)
    uint16_t addr = addrIndexedIndirect();
    aluORA(readByte(addr));
    return 6;
}

unsigned M6502::op11() { // ORA (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    aluORA(readByte(addr));
    return page_crossed ? 6 : 5;
}

// --- EOR ---

unsigned M6502::op49() { // EOR #imm
    uint16_t addr = addrImmediate();
    aluEOR(readByte(addr));
    return 2;
}

unsigned M6502::op45() { // EOR zp
    uint16_t addr = addrZeroPage();
    aluEOR(readByte(addr));
    return 3;
}

unsigned M6502::op55() { // EOR zp,X
    uint16_t addr = addrZeroPageX();
    aluEOR(readByte(addr));
    return 4;
}

unsigned M6502::op4D() { // EOR abs
    uint16_t addr = addrAbsolute();
    aluEOR(readByte(addr));
    return 4;
}

unsigned M6502::op5D() { // EOR abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    aluEOR(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op59() { // EOR abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    aluEOR(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op41() { // EOR (zp,X)
    uint16_t addr = addrIndexedIndirect();
    aluEOR(readByte(addr));
    return 6;
}

unsigned M6502::op51() { // EOR (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    aluEOR(readByte(addr));
    return page_crossed ? 6 : 5;
}

// --- ADC ---

unsigned M6502::op69() { // ADC #imm
    uint16_t addr = addrImmediate();
    aluADC(readByte(addr));
    return 2;
}

unsigned M6502::op65() { // ADC zp
    uint16_t addr = addrZeroPage();
    aluADC(readByte(addr));
    return 3;
}

unsigned M6502::op75() { // ADC zp,X
    uint16_t addr = addrZeroPageX();
    aluADC(readByte(addr));
    return 4;
}

unsigned M6502::op6D() { // ADC abs
    uint16_t addr = addrAbsolute();
    aluADC(readByte(addr));
    return 4;
}

unsigned M6502::op7D() { // ADC abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    aluADC(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op79() { // ADC abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    aluADC(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::op61() { // ADC (zp,X)
    uint16_t addr = addrIndexedIndirect();
    aluADC(readByte(addr));
    return 6;
}

unsigned M6502::op71() { // ADC (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    aluADC(readByte(addr));
    return page_crossed ? 6 : 5;
}

// --- SBC ---

unsigned M6502::opE9() { // SBC #imm
    uint16_t addr = addrImmediate();
    aluSBC(readByte(addr));
    return 2;
}

unsigned M6502::opE5() { // SBC zp
    uint16_t addr = addrZeroPage();
    aluSBC(readByte(addr));
    return 3;
}

unsigned M6502::opF5() { // SBC zp,X
    uint16_t addr = addrZeroPageX();
    aluSBC(readByte(addr));
    return 4;
}

unsigned M6502::opED() { // SBC abs
    uint16_t addr = addrAbsolute();
    aluSBC(readByte(addr));
    return 4;
}

unsigned M6502::opFD() { // SBC abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    aluSBC(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::opF9() { // SBC abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    aluSBC(readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::opE1() { // SBC (zp,X)
    uint16_t addr = addrIndexedIndirect();
    aluSBC(readByte(addr));
    return 6;
}

unsigned M6502::opF1() { // SBC (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    aluSBC(readByte(addr));
    return page_crossed ? 6 : 5;
}
