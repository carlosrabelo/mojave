#include "cpus/m6502.hpp"
#include <cstdio>

unsigned M6502::opUnimplemented(uint8_t op) {
    std::fprintf(stderr, "Unimplemented 6502 opcode 0x%02X at PC=0x%04X\n",
                 static_cast<int>(op), regs_.pc);
    return 2;
}

namespace {

unsigned slo(M6502& c, uint16_t addr, unsigned cycles) {
    uint8_t val = c.shiftASL(c.readByte(addr));
    c.writeByte(addr, val);
    c.aluORA(val);
    return cycles;
}

unsigned rla(M6502& c, uint16_t addr, unsigned cycles) {
    uint8_t val = c.shiftROL(c.readByte(addr));
    c.writeByte(addr, val);
    c.aluAND(val);
    return cycles;
}

unsigned sre(M6502& c, uint16_t addr, unsigned cycles) {
    uint8_t val = c.shiftLSR(c.readByte(addr));
    c.writeByte(addr, val);
    c.aluEOR(val);
    return cycles;
}

unsigned rra(M6502& c, uint16_t addr, unsigned cycles) {
    uint8_t val = c.shiftROR(c.readByte(addr));
    c.writeByte(addr, val);
    c.aluADC(val);
    return cycles;
}

unsigned dcp(M6502& c, uint16_t addr, unsigned cycles) {
    uint8_t val = static_cast<uint8_t>(c.readByte(addr) - 1);
    c.writeByte(addr, val);
    c.aluCompare(c.regs().a, val);
    return cycles;
}

unsigned isb(M6502& c, uint16_t addr, unsigned cycles) {
    uint8_t val = static_cast<uint8_t>(c.readByte(addr) + 1);
    c.writeByte(addr, val);
    c.aluSBC(val);
    return cycles;
}

void lax(M6502& c, uint8_t val) {
    c.regs().a = val;
    c.regs().x = val;
    c.updateNZ(val);
}

void sax(M6502& c, uint16_t addr) {
    c.writeByte(addr, static_cast<uint8_t>(c.regs().a & c.regs().x));
}

uint8_t highAnd(uint16_t addr) {
    return static_cast<uint8_t>((addr >> 8) + 1);
}

} // namespace

// --- KIL / JAM ---

#define KIL(hex) unsigned M6502::op##hex() { is_halted = true; return 2; }
KIL(02) KIL(12) KIL(22) KIL(32) KIL(42) KIL(52)
KIL(62) KIL(72) KIL(92) KIL(B2) KIL(D2) KIL(F2)
#undef KIL

// --- Implied NOP (2 cycles) ---

#define NOP2(hex) unsigned M6502::op##hex() { return 2; }
NOP2(1A) NOP2(3A) NOP2(5A) NOP2(7A) NOP2(DA) NOP2(FA)
#undef NOP2

// --- Immediate NOP (2 cycles) ---

unsigned M6502::op80() { addrImmediate(); return 2; }
unsigned M6502::op82() { addrImmediate(); return 2; }
unsigned M6502::op89() { addrImmediate(); return 2; }
unsigned M6502::opC2() { addrImmediate(); return 2; }
unsigned M6502::opE2() { addrImmediate(); return 2; }

// --- Zero-page NOP (3 cycles) ---

unsigned M6502::op04() { addrZeroPage(); return 3; }
unsigned M6502::op44() { addrZeroPage(); return 3; }
unsigned M6502::op64() { addrZeroPage(); return 3; }

// --- Zero-page,X NOP (4 cycles) ---

unsigned M6502::op14() { addrZeroPageX(); return 4; }
unsigned M6502::op34() { addrZeroPageX(); return 4; }
unsigned M6502::op54() { addrZeroPageX(); return 4; }
unsigned M6502::op74() { addrZeroPageX(); return 4; }
unsigned M6502::opD4() { addrZeroPageX(); return 4; }
unsigned M6502::opF4() { addrZeroPageX(); return 4; }

// --- Absolute NOP (4 cycles) ---

unsigned M6502::op0C() { addrAbsolute(); return 4; }

// --- Absolute,X NOP (4 cycles + page cross) ---

unsigned M6502::op1C() {
    bool page_crossed = false;
    addrAbsoluteX(page_crossed);
    return page_crossed ? 5 : 4;
}
unsigned M6502::op3C() {
    bool page_crossed = false;
    addrAbsoluteX(page_crossed);
    return page_crossed ? 5 : 4;
}
unsigned M6502::op5C() {
    bool page_crossed = false;
    addrAbsoluteX(page_crossed);
    return page_crossed ? 5 : 4;
}
unsigned M6502::op7C() {
    bool page_crossed = false;
    addrAbsoluteX(page_crossed);
    return page_crossed ? 5 : 4;
}
unsigned M6502::opDC() {
    bool page_crossed = false;
    addrAbsoluteX(page_crossed);
    return page_crossed ? 5 : 4;
}
unsigned M6502::opFC() {
    bool page_crossed = false;
    addrAbsoluteX(page_crossed);
    return page_crossed ? 5 : 4;
}

// --- SLO (ASL + ORA) ---

unsigned M6502::op03() { return slo(*this, addrIndexedIndirect(), 8); }
unsigned M6502::op07() { return slo(*this, addrZeroPage(), 5); }
unsigned M6502::op0F() { return slo(*this, addrAbsolute(), 6); }
unsigned M6502::op13() {
    bool page_crossed = false;
    return slo(*this, addrIndirectIndexed(page_crossed), 8);
}
unsigned M6502::op17() { return slo(*this, addrZeroPageX(), 6); }
unsigned M6502::op1B() {
    bool page_crossed = false;
    return slo(*this, addrAbsoluteY(page_crossed), 7);
}
unsigned M6502::op1F() {
    bool page_crossed = false;
    return slo(*this, addrAbsoluteX(page_crossed), 7);
}

// --- RLA (ROL + AND) ---

unsigned M6502::op23() { return rla(*this, addrIndexedIndirect(), 8); }
unsigned M6502::op27() { return rla(*this, addrZeroPage(), 5); }
unsigned M6502::op2F() { return rla(*this, addrAbsolute(), 6); }
unsigned M6502::op33() {
    bool page_crossed = false;
    return rla(*this, addrIndirectIndexed(page_crossed), 8);
}
unsigned M6502::op37() { return rla(*this, addrZeroPageX(), 6); }
unsigned M6502::op3B() {
    bool page_crossed = false;
    return rla(*this, addrAbsoluteY(page_crossed), 7);
}
unsigned M6502::op3F() {
    bool page_crossed = false;
    return rla(*this, addrAbsoluteX(page_crossed), 7);
}

// --- SRE (LSR + EOR) ---

unsigned M6502::op43() { return sre(*this, addrIndexedIndirect(), 8); }
unsigned M6502::op47() { return sre(*this, addrZeroPage(), 5); }
unsigned M6502::op4F() { return sre(*this, addrAbsolute(), 6); }
unsigned M6502::op53() {
    bool page_crossed = false;
    return sre(*this, addrIndirectIndexed(page_crossed), 8);
}
unsigned M6502::op57() { return sre(*this, addrZeroPageX(), 6); }
unsigned M6502::op5B() {
    bool page_crossed = false;
    return sre(*this, addrAbsoluteY(page_crossed), 7);
}
unsigned M6502::op5F() {
    bool page_crossed = false;
    return sre(*this, addrAbsoluteX(page_crossed), 7);
}

// --- RRA (ROR + ADC) ---

unsigned M6502::op63() { return rra(*this, addrIndexedIndirect(), 8); }
unsigned M6502::op67() { return rra(*this, addrZeroPage(), 5); }
unsigned M6502::op6F() { return rra(*this, addrAbsolute(), 6); }
unsigned M6502::op73() {
    bool page_crossed = false;
    return rra(*this, addrIndirectIndexed(page_crossed), 8);
}
unsigned M6502::op77() { return rra(*this, addrZeroPageX(), 6); }
unsigned M6502::op7B() {
    bool page_crossed = false;
    return rra(*this, addrAbsoluteY(page_crossed), 7);
}
unsigned M6502::op7F() {
    bool page_crossed = false;
    return rra(*this, addrAbsoluteX(page_crossed), 7);
}

// --- SAX (store A & X) ---

unsigned M6502::op83() { sax(*this, addrIndexedIndirect()); return 6; }
unsigned M6502::op87() { sax(*this, addrZeroPage()); return 3; }
unsigned M6502::op8F() { sax(*this, addrAbsolute()); return 4; }
unsigned M6502::op97() { sax(*this, addrZeroPageY()); return 4; }

// --- LAX (LDA + LDX) ---

unsigned M6502::opA3() { lax(*this, readByte(addrIndexedIndirect())); return 6; }
unsigned M6502::opA7() { lax(*this, readByte(addrZeroPage())); return 3; }
unsigned M6502::opAF() { lax(*this, readByte(addrAbsolute())); return 4; }
unsigned M6502::opB3() {
    bool page_crossed = false;
    lax(*this, readByte(addrIndirectIndexed(page_crossed)));
    return page_crossed ? 6 : 5;
}
unsigned M6502::opB7() { lax(*this, readByte(addrZeroPageY())); return 4; }
unsigned M6502::opBF() {
    bool page_crossed = false;
    lax(*this, readByte(addrAbsoluteY(page_crossed)));
    return page_crossed ? 5 : 4;
}
unsigned M6502::opAB() { // LAX #imm (unstable); use A = X = imm
    lax(*this, readByte(addrImmediate()));
    return 2;
}

// --- DCP (DEC + CMP) ---

unsigned M6502::opC3() { return dcp(*this, addrIndexedIndirect(), 8); }
unsigned M6502::opC7() { return dcp(*this, addrZeroPage(), 5); }
unsigned M6502::opCF() { return dcp(*this, addrAbsolute(), 6); }
unsigned M6502::opD3() {
    bool page_crossed = false;
    return dcp(*this, addrIndirectIndexed(page_crossed), 8);
}
unsigned M6502::opD7() { return dcp(*this, addrZeroPageX(), 6); }
unsigned M6502::opDB() {
    bool page_crossed = false;
    return dcp(*this, addrAbsoluteY(page_crossed), 7);
}
unsigned M6502::opDF() {
    bool page_crossed = false;
    return dcp(*this, addrAbsoluteX(page_crossed), 7);
}

// --- ISB (INC + SBC) ---

unsigned M6502::opE3() { return isb(*this, addrIndexedIndirect(), 8); }
unsigned M6502::opE7() { return isb(*this, addrZeroPage(), 5); }
unsigned M6502::opEF() { return isb(*this, addrAbsolute(), 6); }
unsigned M6502::opF3() {
    bool page_crossed = false;
    return isb(*this, addrIndirectIndexed(page_crossed), 8);
}
unsigned M6502::opF7() { return isb(*this, addrZeroPageX(), 6); }
unsigned M6502::opFB() {
    bool page_crossed = false;
    return isb(*this, addrAbsoluteY(page_crossed), 7);
}
unsigned M6502::opFF() {
    bool page_crossed = false;
    return isb(*this, addrAbsoluteX(page_crossed), 7);
}

// --- Immediate ALU extras ---

unsigned M6502::op0B() { // ANC #imm
    aluAND(readByte(addrImmediate()));
    setFlagC(getFlagN());
    return 2;
}
unsigned M6502::op2B() { // ANC #imm
    aluAND(readByte(addrImmediate()));
    setFlagC(getFlagN());
    return 2;
}

unsigned M6502::op4B() { // ALR #imm
    aluAND(readByte(addrImmediate()));
    regs_.a = shiftLSR(regs_.a);
    return 2;
}

unsigned M6502::op6B() { // ARR #imm
    aluAND(readByte(addrImmediate()));
    uint8_t res = static_cast<uint8_t>((regs_.a >> 1) | (getFlagC() ? 0x80 : 0));
    setFlagC((res & 0x40) != 0);
    setFlagV(((res & 0x40) != 0) != ((res & 0x20) != 0));
    regs_.a = res;
    updateNZ(res);
    return 2;
}

unsigned M6502::op8B() { // XAA #imm (unstable); A = X & imm
    regs_.a = static_cast<uint8_t>(regs_.x & readByte(addrImmediate()));
    updateNZ(regs_.a);
    return 2;
}

unsigned M6502::opCB() { // AXS / SBX #imm
    uint8_t imm = readByte(addrImmediate());
    uint8_t tmp = static_cast<uint8_t>(regs_.a & regs_.x);
    aluCompare(tmp, imm);
    regs_.x = static_cast<uint8_t>(tmp - imm);
    return 2;
}

unsigned M6502::opEB() { // unofficial SBC #imm
    aluSBC(readByte(addrImmediate()));
    return 2;
}

// --- Unstable high-byte AND stores ---

unsigned M6502::op93() { // SHA (zp),Y
    bool page_crossed = false;
    uint16_t addr = addrIndirectIndexed(page_crossed);
    writeByte(addr, static_cast<uint8_t>(regs_.a & regs_.x & highAnd(addr)));
    return 6;
}
unsigned M6502::op9F() { // SHA abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    writeByte(addr, static_cast<uint8_t>(regs_.a & regs_.x & highAnd(addr)));
    return 5;
}
unsigned M6502::op9C() { // SHY abs,X
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteX(page_crossed);
    writeByte(addr, static_cast<uint8_t>(regs_.y & highAnd(addr)));
    return 5;
}
unsigned M6502::op9E() { // SHX abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    writeByte(addr, static_cast<uint8_t>(regs_.x & highAnd(addr)));
    return 5;
}
unsigned M6502::op9B() { // TAS abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    regs_.sp = static_cast<uint8_t>(regs_.a & regs_.x);
    writeByte(addr, static_cast<uint8_t>(regs_.sp & highAnd(addr)));
    return 5;
}
unsigned M6502::opBB() { // LAS abs,Y
    bool page_crossed = false;
    uint16_t addr = addrAbsoluteY(page_crossed);
    uint8_t val = static_cast<uint8_t>(readByte(addr) & regs_.sp);
    regs_.a = val;
    regs_.x = val;
    regs_.sp = val;
    updateNZ(val);
    return page_crossed ? 5 : 4;
}
