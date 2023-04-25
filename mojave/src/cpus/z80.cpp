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
