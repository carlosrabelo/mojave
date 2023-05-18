#include "cpus/z80.hpp"
#include "bus/bus.hpp"
#include "cpus/z80/dispatch.hpp"
#include <cstdio>
#include <algorithm>

// Phase 6 Control flow, CALL/RET, RST, PUSH/POP (C0–FF)

// 0xC0 - 0xC7
unsigned Z80::opC0() { // RET NZ
    if (evalCondition(0)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opC1() { // POP BC
    regs_.bc = pop16();
    return 10;
}

unsigned Z80::opC2() { // JP NZ, nn
    uint16_t addr = fetchWord();
    if (evalCondition(0)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opC3() { // JP nn
    regs_.pc = fetchWord();
    return 10;
}

unsigned Z80::opC4() { // CALL NZ, nn
    uint16_t addr = fetchWord();
    if (evalCondition(0)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opC5() { // PUSH BC
    push16(regs_.bc);
    return 11;
}

unsigned Z80::opC6() { // ADD A, n
    aluADD(fetchByte());
    return 7;
}

unsigned Z80::opC7() { // RST 00
    push16(regs_.pc);
    regs_.pc = 0x0000;
    return 11;
}

// 0xC8 - 0xCF
unsigned Z80::opC8() { // RET Z
    if (evalCondition(1)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opC9() { // RET
    regs_.pc = pop16();
    return 10;
}

unsigned Z80::opCA() { // JP Z, nn
    uint16_t addr = fetchWord();
    if (evalCondition(1)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opCB_prefix() {
    uint8_t next_op = fetchByte();
    auto handler = z80::kDispatchCB[next_op];
    return (this->*handler)();
}

unsigned Z80::opCC() { // CALL Z, nn
    uint16_t addr = fetchWord();
    if (evalCondition(1)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opCD() { // CALL nn
    uint16_t addr = fetchWord();
    push16(regs_.pc);
    regs_.pc = addr;
    return 17;
}

unsigned Z80::opCE() { // ADC A, n
    aluADC(fetchByte());
    return 7;
}

unsigned Z80::opCF() { // RST 08
    push16(regs_.pc);
    regs_.pc = 0x0008;
    return 11;
}

// 0xD0 - 0xD7
unsigned Z80::opD0() { // RET NC
    if (evalCondition(2)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opD1() { // POP DE
    regs_.de = pop16();
    return 10;
}

unsigned Z80::opD2() { // JP NC, nn
    uint16_t addr = fetchWord();
    if (evalCondition(2)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opD3() { // OUT (n), A
    uint8_t port = fetchByte();
    uint16_t full_port = (getA() << 8) | port;
    if (bus_) {
        bus_->writePort(full_port, getA());
    }
    return 11;
}

unsigned Z80::opD4() { // CALL NC, nn
    uint16_t addr = fetchWord();
    if (evalCondition(2)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opD5() { // PUSH DE
    push16(regs_.de);
    return 11;
}

unsigned Z80::opD6() { // SUB n
    aluSUB(fetchByte());
    return 7;
}

unsigned Z80::opD7() { // RST 10
    push16(regs_.pc);
    regs_.pc = 0x0010;
    return 11;
}

// 0xD8 - 0xDF
unsigned Z80::opD8() { // RET C
    if (evalCondition(3)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opD9() { // EXX
    std::swap(regs_.bc, regs_.bc_);
    std::swap(regs_.de, regs_.de_);
    std::swap(regs_.hl, regs_.hl_);
    return 4;
}

unsigned Z80::opDA() { // JP C, nn
    uint16_t addr = fetchWord();
    if (evalCondition(3)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opDB() { // IN A, (n)
    uint8_t port = fetchByte();
    uint16_t full_port = (getA() << 8) | port;
    if (bus_) {
        setA(bus_->readPort(full_port));
    } else {
        setA(0);
    }
    return 11;
}

unsigned Z80::opDC() { // CALL C, nn
    uint16_t addr = fetchWord();
    if (evalCondition(3)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opDD_prefix() { // DD prefix -> IX indexed addressing
    prefix_dd_ = true;
    prefix_fd_ = false;
    uint8_t next_op = fetchByte();
    return executeDD(next_op);
}

unsigned Z80::opDE() { // SBC A, n
    aluSBC(fetchByte());
    return 7;
}

unsigned Z80::opDF() { // RST 18
    push16(regs_.pc);
    regs_.pc = 0x0018;
    return 11;
}

// 0xE0 - 0xE7
unsigned Z80::opE0() { // RET PO
    if (evalCondition(4)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opE1() { // POP HL
    regs_.hl = pop16();
    return 10;
}

unsigned Z80::opE2() { // JP PO, nn
    uint16_t addr = fetchWord();
    if (evalCondition(4)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opE3() { // EX (SP), HL
    uint8_t low = readByte(regs_.sp);
    uint8_t high = readByte(regs_.sp + 1);
    uint16_t sp_val = (high << 8) | low;
    writeByte(regs_.sp, getL());
    writeByte(regs_.sp + 1, getH());
    regs_.hl = sp_val;
    return 19;
}

unsigned Z80::opE4() { // CALL PO, nn
    uint16_t addr = fetchWord();
    if (evalCondition(4)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opE5() { // PUSH HL
    push16(regs_.hl);
    return 11;
}

unsigned Z80::opE6() { // AND n
    aluAND(fetchByte());
    return 7;
}

unsigned Z80::opE7() { // RST 20
    push16(regs_.pc);
    regs_.pc = 0x0020;
    return 11;
}

// 0xE8 - 0xEF
unsigned Z80::opE8() { // RET PE
    if (evalCondition(5)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opE9() { // JP (HL)
    regs_.pc = regs_.hl;
    return 4;
}

unsigned Z80::opEA() { // JP PE, nn
    uint16_t addr = fetchWord();
    if (evalCondition(5)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opEB() { // EX DE, HL
    std::swap(regs_.de, regs_.hl);
    return 4;
}

unsigned Z80::opEC() { // CALL PE, nn
    uint16_t addr = fetchWord();
    if (evalCondition(5)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opED_prefix() {
    uint8_t next_op = fetchByte();
    auto handler = z80::kDispatchED[next_op];
    return (this->*handler)();
}

unsigned Z80::opEE() { // XOR n
    aluXOR(fetchByte());
    return 7;
}

unsigned Z80::opEF() { // RST 28
    push16(regs_.pc);
    regs_.pc = 0x0028;
    return 11;
}

// 0xF0 - 0xF7
unsigned Z80::opF0() { // RET P
    if (evalCondition(6)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opF1() { // POP AF
    regs_.af = pop16();
    return 10;
}

unsigned Z80::opF2() { // JP P, nn
    uint16_t addr = fetchWord();
    if (evalCondition(6)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opF3() { // DI
    regs_.iff1 = regs_.iff2 = false;
    return 4;
}

unsigned Z80::opF4() { // CALL P, nn
    uint16_t addr = fetchWord();
    if (evalCondition(6)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opF5() { // PUSH AF
    push16(regs_.af);
    return 11;
}

unsigned Z80::opF6() { // OR n
    aluOR(fetchByte());
    return 7;
}

unsigned Z80::opF7() { // RST 30
    push16(regs_.pc);
    regs_.pc = 0x0030;
    return 11;
}

// 0xF8 - 0xFF
unsigned Z80::opF8() { // RET M
    if (evalCondition(7)) {
        regs_.pc = pop16();
        return 11;
    }
    return 5;
}

unsigned Z80::opF9() { // LD SP, HL
    regs_.sp = regs_.hl;
    return 6;
}

unsigned Z80::opFA() { // JP M, nn
    uint16_t addr = fetchWord();
    if (evalCondition(7)) {
        regs_.pc = addr;
    }
    return 10;
}

unsigned Z80::opFB() { // EI - enable after the next instruction completes
    regs_.iff1 = regs_.iff2 = true;
    after_ei_ = true;
    return 4;
}

unsigned Z80::opFC() { // CALL M, nn
    uint16_t addr = fetchWord();
    if (evalCondition(7)) {
        push16(regs_.pc);
        regs_.pc = addr;
        return 17;
    }
    return 10;
}

unsigned Z80::opFD_prefix() { // FD prefix -> IY indexed addressing
    prefix_fd_ = true;
    prefix_dd_ = false;
    uint8_t next_op = fetchByte();
    return executeDD(next_op);
}

unsigned Z80::opFE() { // CP n
    aluCP(fetchByte());
    return 7;
}

unsigned Z80::opFF() { // RST 38
    push16(regs_.pc);
    regs_.pc = 0x0038;
    return 11;
}
