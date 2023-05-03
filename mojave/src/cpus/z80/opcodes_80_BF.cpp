#include "cpus/z80.hpp"

// Phase 5: 8-bit ALU (opcodes 80–BF)

// 0x80 - 0x87: ADD A, r
unsigned Z80::op80() { aluADD(getReg(0)); return 4; }
unsigned Z80::op81() { aluADD(getReg(1)); return 4; }
unsigned Z80::op82() { aluADD(getReg(2)); return 4; }
unsigned Z80::op83() { aluADD(getReg(3)); return 4; }
unsigned Z80::op84() { aluADD(getReg(4)); return 4; }
unsigned Z80::op85() { aluADD(getReg(5)); return 4; }
unsigned Z80::op86() { aluADD(getReg(6)); return 7; }
unsigned Z80::op87() { aluADD(getReg(7)); return 4; }

// 0x88 - 0x8F: ADC A, r
unsigned Z80::op88() { aluADC(getReg(0)); return 4; }
unsigned Z80::op89() { aluADC(getReg(1)); return 4; }
unsigned Z80::op8A() { aluADC(getReg(2)); return 4; }
unsigned Z80::op8B() { aluADC(getReg(3)); return 4; }
unsigned Z80::op8C() { aluADC(getReg(4)); return 4; }
unsigned Z80::op8D() { aluADC(getReg(5)); return 4; }
unsigned Z80::op8E() { aluADC(getReg(6)); return 7; }
unsigned Z80::op8F() { aluADC(getReg(7)); return 4; }

// 0x90 - 0x97: SUB r
unsigned Z80::op90() { aluSUB(getReg(0)); return 4; }
unsigned Z80::op91() { aluSUB(getReg(1)); return 4; }
unsigned Z80::op92() { aluSUB(getReg(2)); return 4; }
unsigned Z80::op93() { aluSUB(getReg(3)); return 4; }
unsigned Z80::op94() { aluSUB(getReg(4)); return 4; }
unsigned Z80::op95() { aluSUB(getReg(5)); return 4; }
unsigned Z80::op96() { aluSUB(getReg(6)); return 7; }
unsigned Z80::op97() { aluSUB(getReg(7)); return 4; }

// 0x98 - 0x9F: SBC A, r
unsigned Z80::op98() { aluSBC(getReg(0)); return 4; }
unsigned Z80::op99() { aluSBC(getReg(1)); return 4; }
unsigned Z80::op9A() { aluSBC(getReg(2)); return 4; }
unsigned Z80::op9B() { aluSBC(getReg(3)); return 4; }
unsigned Z80::op9C() { aluSBC(getReg(4)); return 4; }
unsigned Z80::op9D() { aluSBC(getReg(5)); return 4; }
unsigned Z80::op9E() { aluSBC(getReg(6)); return 7; }
unsigned Z80::op9F() { aluSBC(getReg(7)); return 4; }

// 0xA0 - 0xA7: AND r
unsigned Z80::opA0() { aluAND(getReg(0)); return 4; }
unsigned Z80::opA1() { aluAND(getReg(1)); return 4; }
unsigned Z80::opA2() { aluAND(getReg(2)); return 4; }
unsigned Z80::opA3() { aluAND(getReg(3)); return 4; }
unsigned Z80::opA4() { aluAND(getReg(4)); return 4; }
unsigned Z80::opA5() { aluAND(getReg(5)); return 4; }
unsigned Z80::opA6() { aluAND(getReg(6)); return 7; }
unsigned Z80::opA7() { aluAND(getReg(7)); return 4; }

// 0xA8 - 0xAF: XOR r
unsigned Z80::opA8() { aluXOR(getReg(0)); return 4; }
unsigned Z80::opA9() { aluXOR(getReg(1)); return 4; }
unsigned Z80::opAA() { aluXOR(getReg(2)); return 4; }
unsigned Z80::opAB() { aluXOR(getReg(3)); return 4; }
unsigned Z80::opAC() { aluXOR(getReg(4)); return 4; }
unsigned Z80::opAD() { aluXOR(getReg(5)); return 4; }
unsigned Z80::opAE() { aluXOR(getReg(6)); return 7; }
unsigned Z80::opAF() { aluXOR(getReg(7)); return 4; }

// 0xB0 - 0xB7: OR r
unsigned Z80::opB0() { aluOR(getReg(0)); return 4; }
unsigned Z80::opB1() { aluOR(getReg(1)); return 4; }
unsigned Z80::opB2() { aluOR(getReg(2)); return 4; }
unsigned Z80::opB3() { aluOR(getReg(3)); return 4; }
unsigned Z80::opB4() { aluOR(getReg(4)); return 4; }
unsigned Z80::opB5() { aluOR(getReg(5)); return 4; }
unsigned Z80::opB6() { aluOR(getReg(6)); return 7; }
unsigned Z80::opB7() { aluOR(getReg(7)); return 4; }

// 0xB8 - 0xBF: CP r
unsigned Z80::opB8() { aluCP(getReg(0)); return 4; }
unsigned Z80::opB9() { aluCP(getReg(1)); return 4; }
unsigned Z80::opBA() { aluCP(getReg(2)); return 4; }
unsigned Z80::opBB() { aluCP(getReg(3)); return 4; }
unsigned Z80::opBC() { aluCP(getReg(4)); return 4; }
unsigned Z80::opBD() { aluCP(getReg(5)); return 4; }
unsigned Z80::opBE() { aluCP(getReg(6)); return 7; }
unsigned Z80::opBF() { aluCP(getReg(7)); return 4; }
