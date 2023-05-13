#include "cpus/z80.hpp"
#include "cpus/z80/dispatch.hpp"
#include <cstdio>

void Z80::reset() {
    regs_ = Z80Registers{};
    is_halted = false;
    after_ei_ = false;
    wz_ = 0;
    updatePageTable();
}

unsigned Z80::step() {
    if (is_halted) return 4;
    uint8_t opcode = readByte(regs_.pc++);
    OpcodeHandler handler = z80::kDispatch[opcode];
    return (this->*handler)();
}

unsigned Z80::opUnimplemented() {
    uint16_t pc = regs_.pc - 1;
    uint8_t opcode = readByte(pc);
    std::fprintf(stderr, "Unimplemented Z80 opcode 0x%02X at PC=0x%04X\n", opcode, pc);
    return 4;
}

void Z80::updatePageTable() {
    for (int p = 0; p < 64; ++p) {
        uint16_t start_addr = p * 1024;
        uint16_t end_addr = (p + 1) * 1024 - 1;

        read_pages_[p] = nullptr;
        write_pages_[p] = nullptr;

        if (!bus_) continue;

        auto* m = bus_->find(start_addr);
        if (m && m->device) {
            if (end_addr < m->end_exclusive) {
                uint8_t* dev_ptr = m->device->directPointer(start_addr - m->start);
                if (dev_ptr) {
                    read_pages_[p] = dev_ptr;
                    if (!m->device->isReadOnly()) {
                        write_pages_[p] = dev_ptr;
                    }
                }
            }
        }
    }
}

RegisterSnapshot Z80::registers() const {
    RegisterSnapshot snap;
    snap.entries.push_back(RegisterEntry{"AF", regs_.af});
    snap.entries.push_back(RegisterEntry{"BC", regs_.bc});
    snap.entries.push_back(RegisterEntry{"DE", regs_.de});
    snap.entries.push_back(RegisterEntry{"HL", regs_.hl});
    snap.entries.push_back(RegisterEntry{"AF'", regs_.af_});
    snap.entries.push_back(RegisterEntry{"BC'", regs_.bc_});
    snap.entries.push_back(RegisterEntry{"DE'", regs_.de_});
    snap.entries.push_back(RegisterEntry{"HL'", regs_.hl_});
    snap.entries.push_back(RegisterEntry{"IX", regs_.ix});
    snap.entries.push_back(RegisterEntry{"IY", regs_.iy});
    snap.entries.push_back(RegisterEntry{"SP", regs_.sp});
    snap.entries.push_back(RegisterEntry{"PC", regs_.pc});
    snap.entries.push_back(RegisterEntry{"I", regs_.i});
    snap.entries.push_back(RegisterEntry{"R", regs_.r});
    return snap;
}

void Z80::daa() {
    uint8_t a = getA();
    uint8_t adjust = 0;
    bool carry = getFlagC();
    bool half = getFlagH();
    bool subtract = getFlagN();

    if (subtract) {
        if (carry) {
            adjust |= 0x60;
        }
        if (half) {
            adjust |= 0x06;
        }
        uint16_t result = a - adjust;
        setA(static_cast<uint8_t>(result));
        setFlagC(carry);
    } else {
        if (carry || a > 0x99) {
            adjust |= 0x60;
            carry = true;
        }
        if (half || (a & 0x0F) > 0x09) {
            adjust |= 0x06;
        }
        uint16_t result = a + adjust;
        setA(static_cast<uint8_t>(result));
        setFlagC(carry);
    }

    if (subtract) {
        setFlagH(half && (a & 0x0F) < 6);
    } else {
        setFlagH(((a & 0x0F) + (adjust & 0x0F)) > 0x0F);
    }

    uint8_t new_a = getA();
    setFlagS((new_a & 0x80) != 0);
    setFlagZ(new_a == 0);
    setFlagPV(parity(new_a));
    setF35(new_a);
}

bool Z80::parity(uint8_t val) const {
    unsigned count = 0;
    for (int i = 0; i < 8; ++i) {
        if ((val >> i) & 1) count++;
    }
    return (count % 2) == 0;
}

void Z80::add16(uint16_t& dest, uint16_t src) {
    uint32_t val1 = dest;
    uint32_t val2 = src;
    uint32_t res = val1 + val2;
    dest = static_cast<uint16_t>(res);
    setFlagC(res > 0xFFFF);
    setFlagH(((val1 & 0x0FFF) + (val2 & 0x0FFF)) > 0x0FFF);
    setFlagN(false);
}

uint8_t Z80::inc8(uint8_t v) {
    uint8_t res = v + 1;
    setFlagS((res & 0x80) != 0);
    setFlagZ(res == 0);
    setFlagH((v & 0x0F) == 0x0F);
    setFlagPV(v == 0x7F);
    setFlagN(false);
    setF35(res);
    return res;
}

uint8_t Z80::dec8(uint8_t v) {
    uint8_t res = v - 1;
    setFlagS((res & 0x80) != 0);
    setFlagZ(res == 0);
    setFlagH((v & 0x0F) == 0x00);
    setFlagPV(v == 0x80);
    setFlagN(true);
    setF35(res);
    return res;
}

uint8_t Z80::getReg(int reg_index) {
    switch (reg_index) {
        case 0: return getB();
        case 1: return getC();
        case 2: return getD();
        case 3: return getE();
        case 4: return getH();
        case 5: return getL();
        case 6: return readByte(regs_.hl);
        case 7: return getA();
    }
    return 0;
}

void Z80::setReg(int reg_index, uint8_t val) {
    switch (reg_index) {
        case 0: setB(val); return;
        case 1: setC(val); return;
        case 2: setD(val); return;
        case 3: setE(val); return;
        case 4: setH(val); return;
        case 5: setL(val); return;
        case 6: writeByte(regs_.hl, val); return;
        case 7: setA(val); return;
    }
}

void Z80::aluADD(uint8_t val) {
    uint8_t a = getA();
    uint16_t res = a + val;
    uint8_t r = static_cast<uint8_t>(res);
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH(((a & 0x0F) + (val & 0x0F)) > 0x0F);
    setFlagPV(((a ^ r) & (val ^ r) & 0x80) != 0);
    setFlagN(false);
    setFlagC(res > 0xFF);
    setF35(r);
}

void Z80::aluADC(uint8_t val) {
    uint8_t a = getA();
    uint8_t c = getFlagC() ? 1 : 0;
    uint16_t res = a + val + c;
    uint8_t r = static_cast<uint8_t>(res);
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH(((a & 0x0F) + (val & 0x0F) + c) > 0x0F);
    setFlagPV(((a ^ r) & (val ^ r) & 0x80) != 0);
    setFlagN(false);
    setFlagC(res > 0xFF);
    setF35(r);
}

void Z80::aluSUB(uint8_t val) {
    uint8_t a = getA();
    uint16_t res = a - val;
    uint8_t r = static_cast<uint8_t>(res);
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH((a & 0x0F) < (val & 0x0F));
    setFlagPV(((a ^ val) & (a ^ r) & 0x80) != 0);
    setFlagN(true);
    setFlagC(a < val);
    setF35(r);
}

void Z80::aluSBC(uint8_t val) {
    uint8_t a = getA();
    uint8_t c = getFlagC() ? 1 : 0;
    uint16_t temp = static_cast<uint16_t>(val) + c;
    uint16_t res = a - temp;
    uint8_t r = static_cast<uint8_t>(res);
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH((a & 0x0F) < ((val & 0x0F) + c));
    setFlagPV(((a ^ val) & (a ^ r) & 0x80) != 0);
    setFlagN(true);
    setFlagC(a < temp);
    setF35(r);
}

void Z80::aluAND(uint8_t val) {
    uint8_t r = getA() & val;
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH(true);
    setFlagPV(parity(r));
    setFlagN(false);
    setFlagC(false);
    setF35(r);
}

void Z80::aluXOR(uint8_t val) {
    uint8_t r = getA() ^ val;
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH(false);
    setFlagPV(parity(r));
    setFlagN(false);
    setFlagC(false);
    setF35(r);
}

void Z80::aluOR(uint8_t val) {
    uint8_t r = getA() | val;
    setA(r);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH(false);
    setFlagPV(parity(r));
    setFlagN(false);
    setFlagC(false);
    setF35(r);
}

void Z80::aluCP(uint8_t val) {
    uint8_t a = getA();
    uint16_t res = a - val;
    uint8_t r = static_cast<uint8_t>(res);
    setFlagS((r & 0x80) != 0);
    setFlagZ(r == 0);
    setFlagH((a & 0x0F) < (val & 0x0F));
    setFlagPV(((a ^ val) & (a ^ r) & 0x80) != 0);
    setFlagN(true);
    setFlagC(a < val);
    // CP copies F3/F5 from the operand being compared (not the discarded result).
    setF35(val);
}


void Z80::push16(uint16_t val) {
    regs_.sp -= 2;
    writeByte(regs_.sp, val & 0xFF);
    writeByte(regs_.sp + 1, val >> 8);
}

uint16_t Z80::pop16() {
    uint8_t low = readByte(regs_.sp);
    uint8_t high = readByte(regs_.sp + 1);
    regs_.sp += 2;
    return (high << 8) | low;
}

bool Z80::evalCondition(int cond) const {
    switch (cond) {
        case 0: return !getFlagZ(); // NZ
        case 1: return getFlagZ();  // Z
        case 2: return !getFlagC(); // NC
        case 3: return getFlagC();  // C
        case 4: return !getFlagPV(); // PO
        case 5: return getFlagPV();  // PE
        case 6: return !getFlagS();  // P
        case 7: return getFlagS();   // M
    }
    return false;
}


uint8_t Z80::cbShift(int b, uint8_t val) {
    uint8_t res = 0;
    bool carry = getFlagC();
    switch (b) {
        case 0: // RLC
            setFlagC((val & 0x80) != 0);
            res = (val << 1) | (val >> 7);
            break;
        case 1: // RRC
            setFlagC((val & 1) != 0);
            res = (val >> 1) | (val << 7);
            break;
        case 2: // RL
            setFlagC((val & 0x80) != 0);
            res = (val << 1) | (carry ? 1 : 0);
            break;
        case 3: // RR
            setFlagC((val & 1) != 0);
            res = (val >> 1) | (carry ? 0x80 : 0);
            break;
        case 4: // SLA
            setFlagC((val & 0x80) != 0);
            res = val << 1;
            break;
        case 5: // SRA
            setFlagC((val & 1) != 0);
            res = (val & 0x80) | (val >> 1);
            break;
        case 6: // SLL
            setFlagC((val & 0x80) != 0);
            res = (val << 1) | 1;
            break;
        case 7: // SRL
            setFlagC((val & 1) != 0);
            res = val >> 1;
            break;
    }
    return res;
}


void Z80::edSBC_HL(uint16_t val) {
    uint32_t hl = regs_.hl;
    uint32_t ss = val;
    uint32_t c = getFlagC() ? 1 : 0;
    uint32_t temp = ss + c;
    uint32_t res = hl - temp;
    uint16_t r = static_cast<uint16_t>(res);
    regs_.hl = r;
    setFlagS((r & 0x8000) != 0);
    setFlagZ(r == 0);
    setFlagH((hl & 0x0FFF) < ((ss & 0x0FFF) + c));
    setFlagPV(((hl ^ ss) & (hl ^ r) & 0x8000) != 0);
    setFlagN(true);
    setFlagC(hl < temp);
}

void Z80::edADC_HL(uint16_t val) {
    uint32_t hl = regs_.hl;
    uint32_t ss = val;
    uint32_t c = getFlagC() ? 1 : 0;
    uint32_t res = hl + ss + c;
    uint16_t r = static_cast<uint16_t>(res);
    regs_.hl = r;
    setFlagS((r & 0x8000) != 0);
    setFlagZ(r == 0);
    setFlagH(((hl & 0x0FFF) + (ss & 0x0FFF) + c) > 0x0FFF);
    setFlagPV(((hl ^ r) & (ss ^ r) & 0x8000) != 0);
    setFlagN(false);
    setFlagC(res > 0xFFFF);
}

void Z80::edNEG() {
    uint8_t temp = getA();
    setA(0);
    aluSUB(temp);
}

unsigned Z80::edRETN() {
    regs_.pc = pop16();
    regs_.iff1 = regs_.iff2;
    return 14;
}

void Z80::edLDI() {
    uint8_t val = readByte(regs_.hl);
    writeByte(regs_.de, val);
    regs_.hl++;
    regs_.de++;
    regs_.bc--;
    setFlagH(false);
    setFlagN(false);
    setFlagPV(regs_.bc != 0);
    setF35(val);
}

void Z80::edLDD() {
    uint8_t val = readByte(regs_.hl);
    writeByte(regs_.de, val);
    regs_.hl--;
    regs_.de--;
    regs_.bc--;
    setFlagH(false);
    setFlagN(false);
    setFlagPV(regs_.bc != 0);
    setF35(val);
}

void Z80::edCPI() {
    uint8_t val = readByte(regs_.hl);
    uint8_t res = getA() - val;
    regs_.hl++;
    regs_.bc--;
    setFlagS((res & 0x80) != 0);
    setFlagZ(res == 0);
    setFlagH((getA() & 0x0F) < (val & 0x0F));
    setFlagPV(regs_.bc != 0);
    setFlagN(true);
    setF35(val);
}

void Z80::edCPD() {
    uint8_t val = readByte(regs_.hl);
    uint8_t res = getA() - val;
    regs_.hl--;
    regs_.bc--;
    setFlagS((res & 0x80) != 0);
    setFlagZ(res == 0);
    setFlagH((getA() & 0x0F) < (val & 0x0F));
    setFlagPV(regs_.bc != 0);
    setFlagN(true);
    setF35(val);
}

void Z80::edINI() {
    uint16_t port = regs_.bc;
    uint8_t val = bus_ ? bus_->readPort(port) : 0;
    writeByte(regs_.hl, val);
    regs_.hl++;
    setB(getB() - 1);
    setFlagZ(getB() == 0);
    setFlagN(true);
}

void Z80::edIND() {
    uint16_t port = regs_.bc;
    uint8_t val = bus_ ? bus_->readPort(port) : 0;
    writeByte(regs_.hl, val);
    regs_.hl--;
    setB(getB() - 1);
    setFlagZ(getB() == 0);
    setFlagN(true);
}

void Z80::edOUTI() {
    uint8_t val = readByte(regs_.hl);
    uint16_t port = regs_.bc;
    if (bus_) {
        bus_->writePort(port, val);
    }
    regs_.hl++;
    setB(getB() - 1);
    setFlagZ(getB() == 0);
    setFlagN(true);
}

void Z80::edOUTD() {
    uint8_t val = readByte(regs_.hl);
    uint16_t port = regs_.bc;
    if (bus_) {
        bus_->writePort(port, val);
    }
    regs_.hl--;
    setB(getB() - 1);
    setFlagZ(getB() == 0);
    setFlagN(true);
}

