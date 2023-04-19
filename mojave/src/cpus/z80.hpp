#ifndef MOJAVE_Z80_HPP
#define MOJAVE_Z80_HPP

#include "cpus/cpu.hpp"
#include "bus/bus.hpp"
#include <cstdint>

struct Z80Registers {
    uint16_t af=0, bc=0, de=0, hl=0;
    uint16_t af_=0, bc_=0, de_=0, hl_=0;
    uint16_t ix=0, iy=0;
    uint16_t sp=0, pc=0;
    uint8_t i=0, r=0;
    bool iff1=false, iff2=false;
    unsigned im=0;
};

class Z80 : public Cpu {
public:
    Z80() = default;

    void reset() override;
    unsigned step() override;
    bool halted() const override { return is_halted; }
    RegisterSnapshot registers() const override;

    Z80Registers& regs() { return regs_; }
    const Z80Registers& regs() const { return regs_; }

    inline uint8_t readByte(uint16_t addr) const {
        uint8_t* page_ptr = read_pages_[addr >> 10];
        if (page_ptr) [[likely]] {
            return page_ptr[addr & 0x03FF];
        }
        return bus_ ? bus_->read(addr) : 0;
    }

    inline void writeByte(uint16_t addr, uint8_t val) {
        uint8_t* page_ptr = write_pages_[addr >> 10];
        if (page_ptr) [[likely]] {
            page_ptr[addr & 0x03FF] = val;
            return;
        }
        if (bus_) {
            bus_->write(addr, val);
        }
    }

    void updatePageTable() override;

private:
    Z80Registers regs_{};
    bool is_halted = false;
    uint8_t* read_pages_[64] = {};
    uint8_t* write_pages_[64] = {};
};

#endif
