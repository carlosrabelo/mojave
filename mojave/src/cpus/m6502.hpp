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


    inline void aluADC(uint8_t operand) {
        if (getFlagD()) {
            uint8_t a = regs_.a;
            uint8_t c = getFlagC() ? 1 : 0;

            // Intermediate binary sum for overflow flag
            uint16_t sum_bin = a + operand + c;
            setFlagV(((a ^ sum_bin) & (operand ^ sum_bin) & 0x80) != 0);

            // BCD sum adjustment
            uint16_t low = (a & 0x0F) + (operand & 0x0F) + c;
            if (low > 9) {
                low += 6;
            }

            uint16_t high = (a & 0xF0) + (operand & 0xF0) + (low & 0xF0);
            bool carry_out = false;
            if (high > 0x90) {
                high += 0x60;
                carry_out = true;
            }
            setFlagC(carry_out);

            regs_.a = (high & 0xF0) | (low & 0x0F);
            updateNZ(regs_.a);
        } else {
            uint16_t a = regs_.a;
            uint16_t c = getFlagC() ? 1 : 0;
            uint16_t sum = a + operand + c;

            setFlagC(sum > 0xFF);
            uint8_t result = static_cast<uint8_t>(sum);
            setFlagV(((a ^ result) & (operand ^ result) & 0x80) != 0);
            regs_.a = result;
            updateNZ(regs_.a);
        }
    }

    inline void aluSBC(uint8_t operand) {
        if (getFlagD()) {
            uint8_t a = regs_.a;
            uint8_t c = getFlagC() ? 1 : 0;

            // Intermediate binary difference for carry/overflow flags
            uint16_t diff = a - operand - (1 - c);
            setFlagV(((a ^ diff) & (a ^ operand) & 0x80) != 0);
            setFlagC(diff < 0x100);

            // BCD subtract adjustment
            uint16_t low = (a & 0x0F) - (operand & 0x0F) - (1 - c);
            if (low & 0x10) {
                low -= 6;
            }

            uint16_t high = (a & 0xF0) - (operand & 0xF0) - (low & 0x10);
            if (high & 0x100) {
                high -= 0x60;
            }

            regs_.a = (high & 0xF0) | (low & 0x0F);
            updateNZ(regs_.a);
        } else {
            aluADC(operand ^ 0xFF);
        }
    }

    inline void aluAND(uint8_t operand) {
        regs_.a &= operand;
        updateNZ(regs_.a);
    }

    inline void aluORA(uint8_t operand) {
        regs_.a |= operand;
        updateNZ(regs_.a);
    }

    inline void aluEOR(uint8_t operand) {
        regs_.a ^= operand;
        updateNZ(regs_.a);
    }

    // Compare and BIT helpers
    inline void aluCompare(uint8_t reg, uint8_t operand) {
        uint16_t diff = reg - operand;
        setFlagC(reg >= operand);
        updateNZ(static_cast<uint8_t>(diff));
    }

    inline void aluBIT(uint8_t operand) {
        setFlagZ((regs_.a & operand) == 0);
        setFlagN((operand & 0x80) != 0);
        setFlagV((operand & 0x40) != 0);
    }

    // Shift helpers
    inline uint8_t shiftASL(uint8_t val) {
        setFlagC((val & 0x80) != 0);
        uint8_t res = val << 1;
        updateNZ(res);
        return res;
    }

    inline uint8_t shiftLSR(uint8_t val) {
        setFlagC((val & 0x01) != 0);
        uint8_t res = val >> 1;
        updateNZ(res);
        return res;
    }

    inline uint8_t shiftROL(uint8_t val) {
        bool next_carry = (val & 0x80) != 0;
        uint8_t res = (val << 1) | (getFlagC() ? 1 : 0);
        setFlagC(next_carry);
        updateNZ(res);
        return res;
    }

    inline uint8_t shiftROR(uint8_t val) {
        bool next_carry = (val & 0x01) != 0;
        uint8_t res = (val >> 1) | (getFlagC() ? 0x80 : 0);
        setFlagC(next_carry);
        updateNZ(res);
        return res;
    }

    // Branch helper
    inline unsigned aluBranch(bool condition) {
        int8_t offset = static_cast<int8_t>(readByte(regs_.pc++));
        if (condition) {
            uint16_t old_pc = regs_.pc;
            uint16_t target_pc = regs_.pc + offset;
            regs_.pc = target_pc;
            if ((old_pc & 0xFF00) != (target_pc & 0xFF00)) {
                return 4;
            }
            return 3;
        }
        return 2;
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
