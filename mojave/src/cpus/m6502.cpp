#include "cpus/m6502.hpp"
#include "cpus/m6502/dispatch.hpp"
#include <cstdio>

void M6502::reset() {
    regs_ = M6502Registers{};
    is_halted = false;
    pending_irq = false;
    pending_nmi = false;
    updatePageTable();
    if (bus_) {
        regs_.pc = read16(0xFFFC);
    }
}

unsigned M6502::step() {
    if (is_halted) return 2;

    if (pending_nmi) {
        pending_nmi = false;
        push8(static_cast<uint8_t>(regs_.pc >> 8));
        push8(static_cast<uint8_t>(regs_.pc & 0xFF));
        push8((regs_.p & ~0x10) | 0x20); // B flag clear, U flag set
        setFlagI(true);
        regs_.pc = read16(0xFFFA);
        return 7;
    }

    if (pending_irq && !getFlagI()) {
        pending_irq = false;
        push8(static_cast<uint8_t>(regs_.pc >> 8));
        push8(static_cast<uint8_t>(regs_.pc & 0xFF));
        push8((regs_.p & ~0x10) | 0x20); // B flag clear, U flag set
        setFlagI(true);
        regs_.pc = read16(0xFFFE);
        return 7;
    }

    uint8_t opcode = readByte(regs_.pc++);
    return (this->*m6502::kDispatch[opcode])();
}

void M6502::irq() {
    pending_irq = true;
}

void M6502::nmi() {
    pending_nmi = true;
}

RegisterSnapshot M6502::registers() const {
    RegisterSnapshot snap;
    snap.entries.push_back(RegisterEntry{"A", regs_.a});
    snap.entries.push_back(RegisterEntry{"X", regs_.x});
    snap.entries.push_back(RegisterEntry{"Y", regs_.y});
    snap.entries.push_back(RegisterEntry{"SP", regs_.sp});
    snap.entries.push_back(RegisterEntry{"PC", regs_.pc});
    snap.entries.push_back(RegisterEntry{"P", regs_.p});
    return snap;
}

unsigned M6502::opUnimplemented(uint8_t op) {
    std::fprintf(stderr, "Unimplemented M6502 opcode 0x%02X at PC=0x%04X\n", op,
                 static_cast<uint16_t>(regs_.pc - 1));
    return 2;
}

void M6502::updatePageTable() {
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
