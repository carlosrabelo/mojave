#ifndef MOJAVE_M6507_HPP
#define MOJAVE_M6507_HPP

#include "cpus/m6502.hpp"

class M6507 : public M6502 {
public:
    M6507() = default;

    uint16_t mapAddress(uint16_t addr) const {
        return addr & 0x1FFF;
    }

    uint8_t readByte(uint16_t addr) const override {
        return M6502::readByte(addr & 0x1FFF);
    }

    void writeByte(uint16_t addr, uint8_t val) override {
        M6502::writeByte(addr & 0x1FFF, val);
    }

    // Ignore hardware interrupts (MOS 6507 has no external interrupt pins)
    void irq() {
        // Ignored
    }

    void nmi() {
        // Ignored
    }
};

#endif
