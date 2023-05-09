#include "cpus/z80.hpp"
#include "cpus/z80/dispatch.hpp"

namespace z80 {

const OpcodeHandler kDispatchCB[256] = {
    // 0x00 - 0x0F
    &Z80::opCB00, &Z80::opCB01, &Z80::opCB02, &Z80::opCB03,
    &Z80::opCB04, &Z80::opCB05, &Z80::opCB06, &Z80::opCB07,
    &Z80::opCB08, &Z80::opCB09, &Z80::opCB0A, &Z80::opCB0B,
    &Z80::opCB0C, &Z80::opCB0D, &Z80::opCB0E, &Z80::opCB0F,

    // 0x10 - 0x1F
    &Z80::opCB10, &Z80::opCB11, &Z80::opCB12, &Z80::opCB13,
    &Z80::opCB14, &Z80::opCB15, &Z80::opCB16, &Z80::opCB17,
    &Z80::opCB18, &Z80::opCB19, &Z80::opCB1A, &Z80::opCB1B,
    &Z80::opCB1C, &Z80::opCB1D, &Z80::opCB1E, &Z80::opCB1F,

    // 0x20 - 0x2F
    &Z80::opCB20, &Z80::opCB21, &Z80::opCB22, &Z80::opCB23,
    &Z80::opCB24, &Z80::opCB25, &Z80::opCB26, &Z80::opCB27,
    &Z80::opCB28, &Z80::opCB29, &Z80::opCB2A, &Z80::opCB2B,
    &Z80::opCB2C, &Z80::opCB2D, &Z80::opCB2E, &Z80::opCB2F,

    // 0x30 - 0x3F
    &Z80::opCB30, &Z80::opCB31, &Z80::opCB32, &Z80::opCB33,
    &Z80::opCB34, &Z80::opCB35, &Z80::opCB36, &Z80::opCB37,
    &Z80::opCB38, &Z80::opCB39, &Z80::opCB3A, &Z80::opCB3B,
    &Z80::opCB3C, &Z80::opCB3D, &Z80::opCB3E, &Z80::opCB3F,

    // 0x40 - 0x4F
    &Z80::opCB40, &Z80::opCB41, &Z80::opCB42, &Z80::opCB43,
    &Z80::opCB44, &Z80::opCB45, &Z80::opCB46, &Z80::opCB47,
    &Z80::opCB48, &Z80::opCB49, &Z80::opCB4A, &Z80::opCB4B,
    &Z80::opCB4C, &Z80::opCB4D, &Z80::opCB4E, &Z80::opCB4F,

    // 0x50 - 0x5F
    &Z80::opCB50, &Z80::opCB51, &Z80::opCB52, &Z80::opCB53,
    &Z80::opCB54, &Z80::opCB55, &Z80::opCB56, &Z80::opCB57,
    &Z80::opCB58, &Z80::opCB59, &Z80::opCB5A, &Z80::opCB5B,
    &Z80::opCB5C, &Z80::opCB5D, &Z80::opCB5E, &Z80::opCB5F,

    // 0x60 - 0x6F
    &Z80::opCB60, &Z80::opCB61, &Z80::opCB62, &Z80::opCB63,
    &Z80::opCB64, &Z80::opCB65, &Z80::opCB66, &Z80::opCB67,
    &Z80::opCB68, &Z80::opCB69, &Z80::opCB6A, &Z80::opCB6B,
    &Z80::opCB6C, &Z80::opCB6D, &Z80::opCB6E, &Z80::opCB6F,

    // 0x70 - 0x7F
    &Z80::opCB70, &Z80::opCB71, &Z80::opCB72, &Z80::opCB73,
    &Z80::opCB74, &Z80::opCB75, &Z80::opCB76, &Z80::opCB77,
    &Z80::opCB78, &Z80::opCB79, &Z80::opCB7A, &Z80::opCB7B,
    &Z80::opCB7C, &Z80::opCB7D, &Z80::opCB7E, &Z80::opCB7F,

    // 0x80 - 0x8F
    &Z80::opCB80, &Z80::opCB81, &Z80::opCB82, &Z80::opCB83,
    &Z80::opCB84, &Z80::opCB85, &Z80::opCB86, &Z80::opCB87,
    &Z80::opCB88, &Z80::opCB89, &Z80::opCB8A, &Z80::opCB8B,
    &Z80::opCB8C, &Z80::opCB8D, &Z80::opCB8E, &Z80::opCB8F,

    // 0x90 - 0x9F
    &Z80::opCB90, &Z80::opCB91, &Z80::opCB92, &Z80::opCB93,
    &Z80::opCB94, &Z80::opCB95, &Z80::opCB96, &Z80::opCB97,
    &Z80::opCB98, &Z80::opCB99, &Z80::opCB9A, &Z80::opCB9B,
    &Z80::opCB9C, &Z80::opCB9D, &Z80::opCB9E, &Z80::opCB9F,

    // 0xA0 - 0xAF
    &Z80::opCBA0, &Z80::opCBA1, &Z80::opCBA2, &Z80::opCBA3,
    &Z80::opCBA4, &Z80::opCBA5, &Z80::opCBA6, &Z80::opCBA7,
    &Z80::opCBA8, &Z80::opCBA9, &Z80::opCBAA, &Z80::opCBAB,
    &Z80::opCBAC, &Z80::opCBAD, &Z80::opCBAE, &Z80::opCBAF,

    // 0xB0 - 0xBF
    &Z80::opCBB0, &Z80::opCBB1, &Z80::opCBB2, &Z80::opCBB3,
    &Z80::opCBB4, &Z80::opCBB5, &Z80::opCBB6, &Z80::opCBB7,
    &Z80::opCBB8, &Z80::opCBB9, &Z80::opCBBA, &Z80::opCBBB,
    &Z80::opCBBC, &Z80::opCBBD, &Z80::opCBBE, &Z80::opCBBF,

    // 0xC0 - 0xCF
    &Z80::opCBC0, &Z80::opCBC1, &Z80::opCBC2, &Z80::opCBC3,
    &Z80::opCBC4, &Z80::opCBC5, &Z80::opCBC6, &Z80::opCBC7,
    &Z80::opCBC8, &Z80::opCBC9, &Z80::opCBCA, &Z80::opCBCB,
    &Z80::opCBCC, &Z80::opCBCD, &Z80::opCBCE, &Z80::opCBCF,

    // 0xD0 - 0xDF
    &Z80::opCBD0, &Z80::opCBD1, &Z80::opCBD2, &Z80::opCBD3,
    &Z80::opCBD4, &Z80::opCBD5, &Z80::opCBD6, &Z80::opCBD7,
    &Z80::opCBD8, &Z80::opCBD9, &Z80::opCBDA, &Z80::opCBDB,
    &Z80::opCBDC, &Z80::opCBDD, &Z80::opCBDE, &Z80::opCBDF,

    // 0xE0 - 0xEF
    &Z80::opCBE0, &Z80::opCBE1, &Z80::opCBE2, &Z80::opCBE3,
    &Z80::opCBE4, &Z80::opCBE5, &Z80::opCBE6, &Z80::opCBE7,
    &Z80::opCBE8, &Z80::opCBE9, &Z80::opCBEA, &Z80::opCBEB,
    &Z80::opCBEC, &Z80::opCBED, &Z80::opCBEE, &Z80::opCBEF,

    // 0xF0 - 0xFF
    &Z80::opCBF0, &Z80::opCBF1, &Z80::opCBF2, &Z80::opCBF3,
    &Z80::opCBF4, &Z80::opCBF5, &Z80::opCBF6, &Z80::opCBF7,
    &Z80::opCBF8, &Z80::opCBF9, &Z80::opCBFA, &Z80::opCBFB,
    &Z80::opCBFC, &Z80::opCBFD, &Z80::opCBFE, &Z80::opCBFF
};

} // namespace z80

unsigned Z80::executeCB(uint8_t op) {
    int type = (op >> 6) & 3;
    int b = (op >> 3) & 7;
    int r = op & 7;

    unsigned cycles = 8;
    if (r == 6) {
        cycles = (type == 1) ? 12 : 15;
    }

    uint8_t val = getReg(r);

    if (type == 0) { // Shift/Rotate
        uint8_t res = cbShift(b, val);
        setReg(r, res);
        setFlagS((res & 0x80) != 0);
        setFlagZ(res == 0);
        setFlagH(false);
        setFlagPV(parity(res));
        setFlagN(false);
        setF35(res);
    }
    else if (type == 1) { // BIT
        bool bit_val = (val & (1 << b)) != 0;
        setFlagZ(!bit_val);
        setFlagH(true);
        setFlagN(false);
        if (b == 7) {
            setFlagS(bit_val);
        }
        setFlagPV(!bit_val);
        // F3/F5 come from the tested operand. For BIT n,(HL) the high byte of
        // the address (WZ high = H) drives them instead of the loaded value.
        if (r == 6) {
            wz_ = regs_.hl;
            setF35(static_cast<uint8_t>(wz_ >> 8));
        } else {
            setF35(val);
        }
    }
    else if (type == 2) { // RES
        val &= ~(1 << b);
        setReg(r, val);
    }
    else if (type == 3) { // SET
        val |= (1 << b);
        setReg(r, val);
    }

    return cycles;
}

#define IMPL_CB_ROW(high) \
    unsigned Z80::opCB##high##0() { return executeCB(0x##high##0); } \
    unsigned Z80::opCB##high##1() { return executeCB(0x##high##1); } \
    unsigned Z80::opCB##high##2() { return executeCB(0x##high##2); } \
    unsigned Z80::opCB##high##3() { return executeCB(0x##high##3); } \
    unsigned Z80::opCB##high##4() { return executeCB(0x##high##4); } \
    unsigned Z80::opCB##high##5() { return executeCB(0x##high##5); } \
    unsigned Z80::opCB##high##6() { return executeCB(0x##high##6); } \
    unsigned Z80::opCB##high##7() { return executeCB(0x##high##7); } \
    unsigned Z80::opCB##high##8() { return executeCB(0x##high##8); } \
    unsigned Z80::opCB##high##9() { return executeCB(0x##high##9); } \
    unsigned Z80::opCB##high##A() { return executeCB(0x##high##A); } \
    unsigned Z80::opCB##high##B() { return executeCB(0x##high##B); } \
    unsigned Z80::opCB##high##C() { return executeCB(0x##high##C); } \
    unsigned Z80::opCB##high##D() { return executeCB(0x##high##D); } \
    unsigned Z80::opCB##high##E() { return executeCB(0x##high##E); } \
    unsigned Z80::opCB##high##F() { return executeCB(0x##high##F); }

IMPL_CB_ROW(0) IMPL_CB_ROW(1) IMPL_CB_ROW(2) IMPL_CB_ROW(3)
IMPL_CB_ROW(4) IMPL_CB_ROW(5) IMPL_CB_ROW(6) IMPL_CB_ROW(7)
IMPL_CB_ROW(8) IMPL_CB_ROW(9) IMPL_CB_ROW(A) IMPL_CB_ROW(B)
IMPL_CB_ROW(C) IMPL_CB_ROW(D) IMPL_CB_ROW(E) IMPL_CB_ROW(F)

#undef IMPL_CB_ROW
