#ifndef MOJAVE_M6502_HPP
#define MOJAVE_M6502_HPP

#include "cpus/cpu.hpp"
#include "bus/bus.hpp"
#include <cstdint>

struct M6502Registers {
    uint8_t a=0, x=0, y=0;
    uint8_t sp=0xFD;
    uint16_t pc=0;
    uint8_t p=0x34;
};

class M6502 : public Cpu {
public:
    M6502() = default;

    void reset() override;
    unsigned step() override;
    bool halted() const override { return is_halted; }
    RegisterSnapshot registers() const override;

    M6502Registers& regs() { return regs_; }
    const M6502Registers& regs() const { return regs_; }

    virtual uint8_t readByte(uint16_t addr) const {
        uint8_t* page_ptr = read_pages_[addr >> 10];
        if (page_ptr) [[likely]] {
            return page_ptr[addr & 0x03FF];
        }
        return bus_ ? bus_->read(addr) : 0xEA;
    }

    virtual void writeByte(uint16_t addr, uint8_t val) {
        uint8_t* page_ptr = write_pages_[addr >> 10];
        if (page_ptr) [[likely]] {
            page_ptr[addr & 0x03FF] = val;
            return;
        }
        if (bus_) {
            bus_->write(addr, val);
        }
    }

    inline uint16_t read16(uint16_t addr) const {
        uint8_t low = readByte(addr);
        uint8_t high = readByte(addr + 1);
        return (high << 8) | low;
    }

    void updatePageTable() override;

protected:
    M6502Registers regs_{};
    bool is_halted = false;
    uint8_t* read_pages_[64] = {};
    uint8_t* write_pages_[64] = {};
};

#endif
