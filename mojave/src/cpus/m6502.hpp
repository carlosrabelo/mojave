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

    inline uint16_t read16_zp(uint8_t zp_addr) const {
        uint8_t low = readByte(zp_addr);
        uint8_t high = readByte(static_cast<uint8_t>(zp_addr + 1));
        return (high << 8) | low;
    }

    uint16_t addrImmediate();
    uint16_t addrAbsolute();
    uint16_t addrZeroPage();
    uint16_t addrAbsoluteX(bool& page_crossed);
    uint16_t addrAbsoluteY(bool& page_crossed);
    uint16_t addrZeroPageX();
    uint16_t addrZeroPageY();
    uint16_t addrIndexedIndirect();
    uint16_t addrIndirectIndexed(bool& page_crossed);
    uint16_t addrRelative(bool& page_crossed);
    uint16_t addrIndirect();

    inline bool getFlagN() const { return (regs_.p & 0x80) != 0; }
    inline bool getFlagV() const { return (regs_.p & 0x40) != 0; }
    inline bool getFlagD() const { return (regs_.p & 0x08) != 0; }
    inline bool getFlagI() const { return (regs_.p & 0x04) != 0; }
    inline bool getFlagZ() const { return (regs_.p & 0x02) != 0; }
    inline bool getFlagC() const { return (regs_.p & 0x01) != 0; }

    inline void setFlagN(bool val) { regs_.p = val ? (regs_.p | 0x80) : (regs_.p & ~0x80); }
    inline void setFlagV(bool val) { regs_.p = val ? (regs_.p | 0x40) : (regs_.p & ~0x40); }
    inline void setFlagD(bool val) { regs_.p = val ? (regs_.p | 0x08) : (regs_.p & ~0x08); }
    inline void setFlagI(bool val) { regs_.p = val ? (regs_.p | 0x04) : (regs_.p & ~0x04); }
    inline void setFlagZ(bool val) { regs_.p = val ? (regs_.p | 0x02) : (regs_.p & ~0x02); }
    inline void setFlagC(bool val) { regs_.p = val ? (regs_.p | 0x01) : (regs_.p & ~0x01); }

    inline void updateNZ(uint8_t val) {
        setFlagN((val & 0x80) != 0);
        setFlagZ(val == 0);
    }

    inline void push8(uint8_t val) {
        writeByte(0x0100 | regs_.sp, val);
        regs_.sp--;
    }

    inline uint8_t pop8() {
        regs_.sp++;
        return readByte(0x0100 | regs_.sp);
    }

    void updatePageTable() override;

    using OpcodeHandler = unsigned (M6502::*)();
    unsigned opUnimplemented(uint8_t op);

#define DECL_OP_ROW(high) \
    unsigned op##high##0(), op##high##1(), op##high##2(), op##high##3(), \
             op##high##4(), op##high##5(), op##high##6(), op##high##7(), \
             op##high##8(), op##high##9(), op##high##A(), op##high##B(), \
             op##high##C(), op##high##D(), op##high##E(), op##high##F();

    DECL_OP_ROW(0) DECL_OP_ROW(1) DECL_OP_ROW(2) DECL_OP_ROW(3)
    DECL_OP_ROW(4) DECL_OP_ROW(5) DECL_OP_ROW(6) DECL_OP_ROW(7)
    DECL_OP_ROW(8) DECL_OP_ROW(9) DECL_OP_ROW(A) DECL_OP_ROW(B)
    DECL_OP_ROW(C) DECL_OP_ROW(D) DECL_OP_ROW(E) DECL_OP_ROW(F)

#undef DECL_OP_ROW


protected:
    M6502Registers regs_{};
    bool is_halted = false;
    uint8_t* read_pages_[64] = {};
    uint8_t* write_pages_[64] = {};
};

#include "cpus/m6502/addressing.hpp"

#endif
