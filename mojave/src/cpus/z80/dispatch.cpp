#include "cpus/z80/dispatch.hpp"
#include "cpus/z80.hpp"

namespace z80 {

const OpcodeHandler kDispatch[256] = {
    // 0x00 - 0x0F
    &Z80::op00, &Z80::op01, &Z80::op02, &Z80::op03,
    &Z80::op04, &Z80::op05, &Z80::op06, &Z80::op07,
    &Z80::op08, &Z80::op09, &Z80::op0A, &Z80::op0B,
    &Z80::op0C, &Z80::op0D, &Z80::op0E, &Z80::op0F,

    // 0x10 - 0x1F
    &Z80::op10, &Z80::op11, &Z80::op12, &Z80::op13,
    &Z80::op14, &Z80::op15, &Z80::op16, &Z80::op17,
    &Z80::op18, &Z80::op19, &Z80::op1A, &Z80::op1B,
    &Z80::op1C, &Z80::op1D, &Z80::op1E, &Z80::op1F,

    // 0x20 - 0x2F
    &Z80::op20, &Z80::op21, &Z80::op22, &Z80::op23,
    &Z80::op24, &Z80::op25, &Z80::op26, &Z80::op27,
    &Z80::op28, &Z80::op29, &Z80::op2A, &Z80::op2B,
    &Z80::op2C, &Z80::op2D, &Z80::op2E, &Z80::op2F,

    // 0x30 - 0x3F
    &Z80::op30, &Z80::op31, &Z80::op32, &Z80::op33,
    &Z80::op34, &Z80::op35, &Z80::op36, &Z80::op37,
    &Z80::op38, &Z80::op39, &Z80::op3A, &Z80::op3B,
    &Z80::op3C, &Z80::op3D, &Z80::op3E, &Z80::op3F,

    // 0x40 - 0x4F
    &Z80::op40, &Z80::op41, &Z80::op42, &Z80::op43,
    &Z80::op44, &Z80::op45, &Z80::op46, &Z80::op47,
    &Z80::op48, &Z80::op49, &Z80::op4A, &Z80::op4B,
    &Z80::op4C, &Z80::op4D, &Z80::op4E, &Z80::op4F,

    // 0x50 - 0x5F
    &Z80::op50, &Z80::op51, &Z80::op52, &Z80::op53,
    &Z80::op54, &Z80::op55, &Z80::op56, &Z80::op57,
    &Z80::op58, &Z80::op59, &Z80::op5A, &Z80::op5B,
    &Z80::op5C, &Z80::op5D, &Z80::op5E, &Z80::op5F,

    // 0x60 - 0x6F
    &Z80::op60, &Z80::op61, &Z80::op62, &Z80::op63,
    &Z80::op64, &Z80::op65, &Z80::op66, &Z80::op67,
    &Z80::op68, &Z80::op69, &Z80::op6A, &Z80::op6B,
    &Z80::op6C, &Z80::op6D, &Z80::op6E, &Z80::op6F,

    // 0x70 - 0x7F
    &Z80::op70, &Z80::op71, &Z80::op72, &Z80::op73,
    &Z80::op74, &Z80::op75, &Z80::op76, &Z80::op77,
    &Z80::op78, &Z80::op79, &Z80::op7A, &Z80::op7B,
    &Z80::op7C, &Z80::op7D, &Z80::op7E, &Z80::op7F,

    // 0x80 - 0x8F
    &Z80::op80, &Z80::op81, &Z80::op82, &Z80::op83,
    &Z80::op84, &Z80::op85, &Z80::op86, &Z80::op87,
    &Z80::op88, &Z80::op89, &Z80::op8A, &Z80::op8B,
    &Z80::op8C, &Z80::op8D, &Z80::op8E, &Z80::op8F,

    // 0x90 - 0x9F
    &Z80::op90, &Z80::op91, &Z80::op92, &Z80::op93,
    &Z80::op94, &Z80::op95, &Z80::op96, &Z80::op97,
    &Z80::op98, &Z80::op99, &Z80::op9A, &Z80::op9B,
    &Z80::op9C, &Z80::op9D, &Z80::op9E, &Z80::op9F,

    // 0xA0 - 0xAF
    &Z80::opA0, &Z80::opA1, &Z80::opA2, &Z80::opA3,
    &Z80::opA4, &Z80::opA5, &Z80::opA6, &Z80::opA7,
    &Z80::opA8, &Z80::opA9, &Z80::opAA, &Z80::opAB,
    &Z80::opAC, &Z80::opAD, &Z80::opAE, &Z80::opAF,

    // 0xB0 - 0xBF
    &Z80::opB0, &Z80::opB1, &Z80::opB2, &Z80::opB3,
    &Z80::opB4, &Z80::opB5, &Z80::opB6, &Z80::opB7,
    &Z80::opB8, &Z80::opB9, &Z80::opBA, &Z80::opBB,
    &Z80::opBC, &Z80::opBD, &Z80::opBE, &Z80::opBF,

    // 0xC0 - 0xCF
    &Z80::opC0, &Z80::opC1, &Z80::opC2, &Z80::opC3,
    &Z80::opC4, &Z80::opC5, &Z80::opC6, &Z80::opC7,
    &Z80::opC8, &Z80::opC9, &Z80::opCA, &Z80::opCB_prefix,
    &Z80::opCC, &Z80::opCD, &Z80::opCE, &Z80::opCF,

    // 0xD0 - 0xDF
    &Z80::opD0, &Z80::opD1, &Z80::opD2, &Z80::opD3,
    &Z80::opD4, &Z80::opD5, &Z80::opD6, &Z80::opD7,
    &Z80::opD8, &Z80::opD9, &Z80::opDA, &Z80::opDB,
    &Z80::opDC, &Z80::opDD_prefix, &Z80::opDE, &Z80::opDF,

    // 0xE0 - 0xEF
    &Z80::opE0, &Z80::opE1, &Z80::opE2, &Z80::opE3,
    &Z80::opE4, &Z80::opE5, &Z80::opE6, &Z80::opE7,
    &Z80::opE8, &Z80::opE9, &Z80::opEA, &Z80::opEB,
    &Z80::opEC, &Z80::opED_prefix, &Z80::opEE, &Z80::opEF,

    // 0xF0 - 0xFF
    &Z80::opF0, &Z80::opF1, &Z80::opF2, &Z80::opF3,
    &Z80::opF4, &Z80::opF5, &Z80::opF6, &Z80::opF7,
    &Z80::opF8, &Z80::opF9, &Z80::opFA, &Z80::opFB,
    &Z80::opFC, &Z80::opFD_prefix, &Z80::opFE, &Z80::opFF
};

} // namespace z80
