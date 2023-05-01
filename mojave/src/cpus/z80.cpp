#include "cpus/z80.hpp"
#include "cpus/z80/dispatch.hpp"
#include <cstdio>

void Z80::reset() {
    regs_ = Z80Registers{};
    is_halted = false;
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
