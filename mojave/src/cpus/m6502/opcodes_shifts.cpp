#include "cpus/m6502.hpp"

// --- COMPARE ---

// CMP
unsigned M6502::opC9() { // CMP #imm
    uint16_t addr = addrImmediate();
    aluCompare(regs_.a, readByte(addr));
    return 2;
}

unsigned M6502::opC5() { // CMP zp
    uint16_t addr = addrZeroPage();
    aluCompare(regs_.a, readByte(addr));
    return 3;
}

unsigned M6502::opD5() { // CMP zp,X
    uint16_t addr = addrZeroPageX();
    aluCompare(regs_.a, readByte(addr));
    return 4;
}

unsigned M6502::opCD() { // CMP abs
    uint16_t addr = addrAbsolute();
    aluCompare(regs_.a, readByte(addr));
    return 4;
}

unsigned M6502::opDD() { // CMP abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    aluCompare(regs_.a, readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::opD9() { // CMP abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    aluCompare(regs_.a, readByte(addr));
    return page_crossed ? 5 : 4;
}

unsigned M6502::opC1() { // CMP (zp,X)
    uint16_t addr = addrIndexedIndirect();
    aluCompare(regs_.a, readByte(addr));
    return 6;
}

unsigned M6502::opD1() { // CMP (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    aluCompare(regs_.a, readByte(addr));
    return page_crossed ? 6 : 5;
}

// CPX
unsigned M6502::opE0() { // CPX #imm
    uint16_t addr = addrImmediate();
    aluCompare(regs_.x, readByte(addr));
    return 2;
}

unsigned M6502::opE4() { // CPX zp
    uint16_t addr = addrZeroPage();
    aluCompare(regs_.x, readByte(addr));
    return 3;
}

unsigned M6502::opEC() { // CPX abs
    uint16_t addr = addrAbsolute();
    aluCompare(regs_.x, readByte(addr));
    return 4;
}

// CPY
unsigned M6502::opC0() { // CPY #imm
    uint16_t addr = addrImmediate();
    aluCompare(regs_.y, readByte(addr));
    return 2;
}

unsigned M6502::opC4() { // CPY zp
    uint16_t addr = addrZeroPage();
    aluCompare(regs_.y, readByte(addr));
    return 3;
}

unsigned M6502::opCC() { // CPY abs
    uint16_t addr = addrAbsolute();
    aluCompare(regs_.y, readByte(addr));
    return 4;
}

// --- BIT ---

unsigned M6502::op24() { // BIT zp
    uint16_t addr = addrZeroPage();
    aluBIT(readByte(addr));
    return 3;
}

unsigned M6502::op2C() { // BIT abs
    uint16_t addr = addrAbsolute();
    aluBIT(readByte(addr));
    return 4;
}

// --- INCREMENTS / DECREMENTS ---

// Register
unsigned M6502::opE8() { // INX
    regs_.x++;
    updateNZ(regs_.x);
    return 2;
}

unsigned M6502::opCA() { // DEX
    regs_.x--;
    updateNZ(regs_.x);
    return 2;
}

unsigned M6502::opC8() { // INY
    regs_.y++;
    updateNZ(regs_.y);
    return 2;
}

unsigned M6502::op88() { // DEY
    regs_.y--;
    updateNZ(regs_.y);
    return 2;
}

// Memory INC
unsigned M6502::opE6() { // INC zp
    uint16_t addr = addrZeroPage();
    uint8_t val = readByte(addr) + 1;
    writeByte(addr, val);
    updateNZ(val);
    return 5;
}

unsigned M6502::opF6() { // INC zp,X
    uint16_t addr = addrZeroPageX();
    uint8_t val = readByte(addr) + 1;
    writeByte(addr, val);
    updateNZ(val);
    return 6;
}

unsigned M6502::opEE() { // INC abs
    uint16_t addr = addrAbsolute();
    uint8_t val = readByte(addr) + 1;
    writeByte(addr, val);
    updateNZ(val);
    return 6;
}

unsigned M6502::opFE() { // INC abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    uint8_t val = readByte(addr) + 1;
    writeByte(addr, val);
    updateNZ(val);
    return 7;
}

// Memory DEC
unsigned M6502::opC6() { // DEC zp
    uint16_t addr = addrZeroPage();
    uint8_t val = readByte(addr) - 1;
    writeByte(addr, val);
    updateNZ(val);
    return 5;
}

unsigned M6502::opD6() { // DEC zp,X
    uint16_t addr = addrZeroPageX();
    uint8_t val = readByte(addr) - 1;
    writeByte(addr, val);
    updateNZ(val);
    return 6;
}

unsigned M6502::opCE() { // DEC abs
    uint16_t addr = addrAbsolute();
    uint8_t val = readByte(addr) - 1;
    writeByte(addr, val);
    updateNZ(val);
    return 6;
}

unsigned M6502::opDE() { // DEC abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    uint8_t val = readByte(addr) - 1;
    writeByte(addr, val);
    updateNZ(val);
    return 7;
}

// --- SHIFTS ---

// ASL
unsigned M6502::op0A() { // ASL A
    regs_.a = shiftASL(regs_.a);
    return 2;
}

unsigned M6502::op06() { // ASL zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, shiftASL(readByte(addr)));
    return 5;
}

unsigned M6502::op16() { // ASL zp,X
    uint16_t addr = addrZeroPageX();
    writeByte(addr, shiftASL(readByte(addr)));
    return 6;
}

unsigned M6502::op0E() { // ASL abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, shiftASL(readByte(addr)));
    return 6;
}

unsigned M6502::op1E() { // ASL abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    writeByte(addr, shiftASL(readByte(addr)));
    return 7;
}

// LSR
unsigned M6502::op4A() { // LSR A
    regs_.a = shiftLSR(regs_.a);
    return 2;
}

unsigned M6502::op46() { // LSR zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, shiftLSR(readByte(addr)));
    return 5;
}

unsigned M6502::op56() { // LSR zp,X
    uint16_t addr = addrZeroPageX();
    writeByte(addr, shiftLSR(readByte(addr)));
    return 6;
}

unsigned M6502::op4E() { // LSR abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, shiftLSR(readByte(addr)));
    return 6;
}

unsigned M6502::op5E() { // LSR abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    writeByte(addr, shiftLSR(readByte(addr)));
    return 7;
}

// ROL
unsigned M6502::op2A() { // ROL A
    regs_.a = shiftROL(regs_.a);
    return 2;
}

unsigned M6502::op26() { // ROL zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, shiftROL(readByte(addr)));
    return 5;
}

unsigned M6502::op36() { // ROL zp,X
    uint16_t addr = addrZeroPageX();
    writeByte(addr, shiftROL(readByte(addr)));
    return 6;
}

unsigned M6502::op2E() { // ROL abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, shiftROL(readByte(addr)));
    return 6;
}

unsigned M6502::op3E() { // ROL abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    writeByte(addr, shiftROL(readByte(addr)));
    return 7;
}

// ROR
unsigned M6502::op6A() { // ROR A
    regs_.a = shiftROR(regs_.a);
    return 2;
}

unsigned M6502::op66() { // ROR zp
    uint16_t addr = addrZeroPage();
    writeByte(addr, shiftROR(readByte(addr)));
    return 5;
}

unsigned M6502::op76() { // ROR zp,X
    uint16_t addr = addrZeroPageX();
    writeByte(addr, shiftROR(readByte(addr)));
    return 6;
}

unsigned M6502::op6E() { // ROR abs
    uint16_t addr = addrAbsolute();
    writeByte(addr, shiftROR(readByte(addr)));
    return 6;
}

unsigned M6502::op7E() { // ROR abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    writeByte(addr, shiftROR(readByte(addr)));
    return 7;
}
