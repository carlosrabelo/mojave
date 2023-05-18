#ifndef MOJAVE_Z80_HPP
#define MOJAVE_Z80_HPP

#include "cpus/cpu.hpp"
#include "cpus/z80/opcodes.hpp"
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

    uint16_t wz() const { return wz_; }
    void setWz(uint16_t v) { wz_ = v; }

    unsigned step() override;
    bool halted() const override { return is_halted; }
    RegisterSnapshot registers() const override;

    Z80Registers& regs() { return regs_; }
    const Z80Registers& regs() const { return regs_; }


    // Register accessors (high/low parts)
    uint8_t getA() const { return regs_.af >> 8; }
    void setA(uint8_t val) { regs_.af = (val << 8) | (regs_.af & 0x00FF); }
    uint8_t getF() const { return regs_.af & 0xFF; }
    void setF(uint8_t val) { regs_.af = (regs_.af & 0xFF00) | val; }

    uint8_t getB() const { return regs_.bc >> 8; }
    void setB(uint8_t val) { regs_.bc = (val << 8) | (regs_.bc & 0x00FF); }
    uint8_t getC() const { return regs_.bc & 0xFF; }
    void setC(uint8_t val) { regs_.bc = (regs_.bc & 0xFF00) | val; }

    uint8_t getD() const { return regs_.de >> 8; }
    void setD(uint8_t val) { regs_.de = (val << 8) | (regs_.de & 0x00FF); }
    uint8_t getE() const { return regs_.de & 0xFF; }
    void setE(uint8_t val) { regs_.de = (regs_.de & 0xFF00) | val; }

    uint8_t getH() const { return regs_.hl >> 8; }
    void setH(uint8_t val) { regs_.hl = (val << 8) | (regs_.hl & 0x00FF); }
    uint8_t getL() const { return regs_.hl & 0xFF; }
    void setL(uint8_t val) { regs_.hl = (regs_.hl & 0xFF00) | val; }

    bool getFlagS() const { return (getF() & 0x80) != 0; }
    bool getFlagZ() const { return (getF() & 0x40) != 0; }
    bool getFlagH() const { return (getF() & 0x10) != 0; }
    bool getFlagPV() const { return (getF() & 0x04) != 0; }
    bool getFlagN() const { return (getF() & 0x02) != 0; }
    bool getFlagC() const { return (getF() & 0x01) != 0; }
    bool getFlagF5() const { return (getF() & 0x20) != 0; }
    bool getFlagF3() const { return (getF() & 0x08) != 0; }

    void setFlagS(bool val) { setF(val ? (getF() | 0x80) : (getF() & ~0x80)); }
    void setFlagZ(bool val) { setF(val ? (getF() | 0x40) : (getF() & ~0x40)); }
    void setFlagH(bool val) { setF(val ? (getF() | 0x10) : (getF() & ~0x10)); }
    void setFlagPV(bool val) { setF(val ? (getF() | 0x04) : (getF() & ~0x04)); }
    void setFlagN(bool val) { setF(val ? (getF() | 0x02) : (getF() & ~0x02)); }
    void setFlagC(bool val) { setF(val ? (getF() | 0x01) : (getF() & ~0x01)); }
    void setFlagF5(bool val) { setF(val ? (getF() | 0x20) : (getF() & ~0x20)); }
    void setFlagF3(bool val) { setF(val ? (getF() | 0x08) : (getF() & ~0x08)); }
    void setF35(uint8_t val) {
        setFlagF3((val & 0x08) != 0);
        setFlagF5((val & 0x20) != 0);
    }

    uint8_t fetchByte() { return readByte(regs_.pc++); }
    uint16_t fetchWord() {
        uint8_t low = fetchByte();
        uint8_t high = fetchByte();
        return (high << 8) | low;
    }

    bool parity(uint8_t val) const;
    void daa();
    void add16(uint16_t& dest, uint16_t src);
    uint8_t inc8(uint8_t v);
    uint8_t dec8(uint8_t v);

    uint8_t getReg(int reg_index);
    void setReg(int reg_index, uint8_t val);

    void aluADD(uint8_t val);
    void aluADC(uint8_t val);
    void aluSUB(uint8_t val);
    void aluSBC(uint8_t val);
    void aluAND(uint8_t val);
    void aluXOR(uint8_t val);
    void aluOR(uint8_t val);
    void aluCP(uint8_t val);

    void push16(uint16_t val);
    uint16_t pop16();
    bool evalCondition(int cond) const;

    unsigned executeCB(uint8_t op);
    uint8_t cbShift(int b, uint8_t val);

    void edSBC_HL(uint16_t val);
    void edADC_HL(uint16_t val);
    void edNEG();
    unsigned edRETN();
    void edLDI();
    void edLDD();
    void edCPI();
    void edCPD();
    void edINI();
    void edIND();
    void edOUTI();
    void edOUTD();

    unsigned executeED(uint8_t op);

    uint16_t& indexReg();
    int8_t fetchDisp() { return static_cast<int8_t>(fetchByte()); }
    void incIndexHalf(int idx);
    void decIndexHalf(int idx);
    void incIndexedMem();
    void decIndexedMem();
    unsigned executeDD(uint8_t op);
    unsigned executeDDCB();


#define DECL_ED_ROW(high) \
    unsigned opED##high##0(), opED##high##1(), opED##high##2(), opED##high##3(), \
             opED##high##4(), opED##high##5(), opED##high##6(), opED##high##7(), \
             opED##high##8(), opED##high##9(), opED##high##A(), opED##high##B(), \
             opED##high##C(), opED##high##D(), opED##high##E(), opED##high##F();

    DECL_ED_ROW(0) DECL_ED_ROW(1) DECL_ED_ROW(2) DECL_ED_ROW(3)
    DECL_ED_ROW(4) DECL_ED_ROW(5) DECL_ED_ROW(6) DECL_ED_ROW(7)
    DECL_ED_ROW(8) DECL_ED_ROW(9) DECL_ED_ROW(A) DECL_ED_ROW(B)
    DECL_ED_ROW(C) DECL_ED_ROW(D) DECL_ED_ROW(E) DECL_ED_ROW(F)

#undef DECL_ED_ROW


#define DECL_CB_ROW(high) \
    unsigned opCB##high##0(), opCB##high##1(), opCB##high##2(), opCB##high##3(), \
             opCB##high##4(), opCB##high##5(), opCB##high##6(), opCB##high##7(), \
             opCB##high##8(), opCB##high##9(), opCB##high##A(), opCB##high##B(), \
             opCB##high##C(), opCB##high##D(), opCB##high##E(), opCB##high##F();

    DECL_CB_ROW(0) DECL_CB_ROW(1) DECL_CB_ROW(2) DECL_CB_ROW(3)
    DECL_CB_ROW(4) DECL_CB_ROW(5) DECL_CB_ROW(6) DECL_CB_ROW(7)
    DECL_CB_ROW(8) DECL_CB_ROW(9) DECL_CB_ROW(A) DECL_CB_ROW(B)
    DECL_CB_ROW(C) DECL_CB_ROW(D) DECL_CB_ROW(E) DECL_CB_ROW(F)

#undef DECL_CB_ROW



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


    using OpcodeHandler = z80::OpcodeHandler;

    unsigned opUnimplemented();

    unsigned op00(), op01(), op02(), op03(), op04(), op05(), op06(), op07();
    unsigned op08(), op09(), op0A(), op0B(), op0C(), op0D(), op0E(), op0F();
    unsigned op10(), op11(), op12(), op13(), op14(), op15(), op16(), op17();
    unsigned op18(), op19(), op1A(), op1B(), op1C(), op1D(), op1E(), op1F();
    unsigned op20(), op21(), op22(), op23(), op24(), op25(), op26(), op27();
    unsigned op28(), op29(), op2A(), op2B(), op2C(), op2D(), op2E(), op2F();
    unsigned op30(), op31(), op32(), op33(), op34(), op35(), op36(), op37();
    unsigned op38(), op39(), op3A(), op3B(), op3C(), op3D(), op3E(), op3F();

    unsigned op40(), op41(), op42(), op43(), op44(), op45(), op46(), op47();
    unsigned op48(), op49(), op4A(), op4B(), op4C(), op4D(), op4E(), op4F();
    unsigned op50(), op51(), op52(), op53(), op54(), op55(), op56(), op57();
    unsigned op58(), op59(), op5A(), op5B(), op5C(), op5D(), op5E(), op5F();
    unsigned op60(), op61(), op62(), op63(), op64(), op65(), op66(), op67();
    unsigned op68(), op69(), op6A(), op6B(), op6C(), op6D(), op6E(), op6F();
    unsigned op70(), op71(), op72(), op73(), op74(), op75(), op76(), op77();
    unsigned op78(), op79(), op7A(), op7B(), op7C(), op7D(), op7E(), op7F();

    unsigned op80(), op81(), op82(), op83(), op84(), op85(), op86(), op87();
    unsigned op88(), op89(), op8A(), op8B(), op8C(), op8D(), op8E(), op8F();
    unsigned op90(), op91(), op92(), op93(), op94(), op95(), op96(), op97();
    unsigned op98(), op99(), op9A(), op9B(), op9C(), op9D(), op9E(), op9F();
    unsigned opA0(), opA1(), opA2(), opA3(), opA4(), opA5(), opA6(), opA7();
    unsigned opA8(), opA9(), opAA(), opAB(), opAC(), opAD(), opAE(), opAF();
    unsigned opB0(), opB1(), opB2(), opB3(), opB4(), opB5(), opB6(), opB7();
    unsigned opB8(), opB9(), opBA(), opBB(), opBC(), opBD(), opBE(), opBF();

    unsigned opC0(), opC1(), opC2(), opC3(), opC4(), opC5(), opC6(), opC7();
    unsigned opC8(), opC9(), opCA(), opCB_prefix(), opCC(), opCD(), opCE(), opCF();
    unsigned opD0(), opD1(), opD2(), opD3(), opD4(), opD5(), opD6(), opD7();
    unsigned opD8(), opD9(), opDA(), opDB(), opDC(), opDD_prefix(), opDE(), opDF();
    unsigned opE0(), opE1(), opE2(), opE3(), opE4(), opE5(), opE6(), opE7();
    unsigned opE8(), opE9(), opEA(), opEB(), opEC(), opED_prefix(), opEE(), opEF();
    unsigned opF0(), opF1(), opF2(), opF3(), opF4(), opF5(), opF6(), opF7();
    unsigned opF8(), opF9(), opFA(), opFB(), opFC(), opFD_prefix(), opFE(), opFF();

    void updatePageTable() override;

private:
    Z80Registers regs_{};
    bool is_halted = false;
    bool prefix_dd_ = false;
    bool prefix_fd_ = false;
    bool after_ei_ = false;
    uint16_t wz_ = 0;
    uint8_t* read_pages_[64] = {};
    uint8_t* write_pages_[64] = {};
};

#endif
