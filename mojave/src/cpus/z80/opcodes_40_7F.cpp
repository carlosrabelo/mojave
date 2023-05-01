#include "cpus/z80.hpp"

// Phase 4 Register-to-register loads (opcodes 40–7F) and HALT

unsigned Z80::op40() { setReg(0, getReg(0)); return 4; } // LD B,B
unsigned Z80::op41() { setReg(0, getReg(1)); return 4; } // LD B,C
unsigned Z80::op42() { setReg(0, getReg(2)); return 4; } // LD B,D
unsigned Z80::op43() { setReg(0, getReg(3)); return 4; } // LD B,E
unsigned Z80::op44() { setReg(0, getReg(4)); return 4; } // LD B,H
unsigned Z80::op45() { setReg(0, getReg(5)); return 4; } // LD B,L
unsigned Z80::op46() { setReg(0, getReg(6)); return 7; } // LD B,(HL)
unsigned Z80::op47() { setReg(0, getReg(7)); return 4; } // LD B,A

unsigned Z80::op48() { setReg(1, getReg(0)); return 4; } // LD C,B
unsigned Z80::op49() { setReg(1, getReg(1)); return 4; } // LD C,C
unsigned Z80::op4A() { setReg(1, getReg(2)); return 4; } // LD C,D
unsigned Z80::op4B() { setReg(1, getReg(3)); return 4; } // LD C,E
unsigned Z80::op4C() { setReg(1, getReg(4)); return 4; } // LD C,H
unsigned Z80::op4D() { setReg(1, getReg(5)); return 4; } // LD C,L
unsigned Z80::op4E() { setReg(1, getReg(6)); return 7; } // LD C,(HL)
unsigned Z80::op4F() { setReg(1, getReg(7)); return 4; } // LD C,A

unsigned Z80::op50() { setReg(2, getReg(0)); return 4; } // LD D,B
unsigned Z80::op51() { setReg(2, getReg(1)); return 4; } // LD D,C
unsigned Z80::op52() { setReg(2, getReg(2)); return 4; } // LD D,D
unsigned Z80::op53() { setReg(2, getReg(3)); return 4; } // LD D,E
unsigned Z80::op54() { setReg(2, getReg(4)); return 4; } // LD D,H
unsigned Z80::op55() { setReg(2, getReg(5)); return 4; } // LD D,L
unsigned Z80::op56() { setReg(2, getReg(6)); return 7; } // LD D,(HL)
unsigned Z80::op57() { setReg(2, getReg(7)); return 4; } // LD D,A

unsigned Z80::op58() { setReg(3, getReg(0)); return 4; } // LD E,B
unsigned Z80::op59() { setReg(3, getReg(1)); return 4; } // LD E,C
unsigned Z80::op5A() { setReg(3, getReg(2)); return 4; } // LD E,D
unsigned Z80::op5B() { setReg(3, getReg(3)); return 4; } // LD E,E
unsigned Z80::op5C() { setReg(3, getReg(4)); return 4; } // LD E,H
unsigned Z80::op5D() { setReg(3, getReg(5)); return 4; } // LD E,L
unsigned Z80::op5E() { setReg(3, getReg(6)); return 7; } // LD E,(HL)
unsigned Z80::op5F() { setReg(3, getReg(7)); return 4; } // LD E,A

unsigned Z80::op60() { setReg(4, getReg(0)); return 4; } // LD H,B
unsigned Z80::op61() { setReg(4, getReg(1)); return 4; } // LD H,C
unsigned Z80::op62() { setReg(4, getReg(2)); return 4; } // LD H,D
unsigned Z80::op63() { setReg(4, getReg(3)); return 4; } // LD H,E
unsigned Z80::op64() { setReg(4, getReg(4)); return 4; } // LD H,H
unsigned Z80::op65() { setReg(4, getReg(5)); return 4; } // LD H,L
unsigned Z80::op66() { setReg(4, getReg(6)); return 7; } // LD H,(HL)
unsigned Z80::op67() { setReg(4, getReg(7)); return 4; } // LD H,A

unsigned Z80::op68() { setReg(5, getReg(0)); return 4; } // LD L,B
unsigned Z80::op69() { setReg(5, getReg(1)); return 4; } // LD L,C
unsigned Z80::op6A() { setReg(5, getReg(2)); return 4; } // LD L,D
unsigned Z80::op6B() { setReg(5, getReg(3)); return 4; } // LD L,E
unsigned Z80::op6C() { setReg(5, getReg(4)); return 4; } // LD L,H
unsigned Z80::op6D() { setReg(5, getReg(5)); return 4; } // LD L,L
unsigned Z80::op6E() { setReg(5, getReg(6)); return 7; } // LD L,(HL)
unsigned Z80::op6F() { setReg(5, getReg(7)); return 4; } // LD L,A

unsigned Z80::op70() { setReg(6, getReg(0)); return 7; } // LD (HL),B
unsigned Z80::op71() { setReg(6, getReg(1)); return 7; } // LD (HL),C
unsigned Z80::op72() { setReg(6, getReg(2)); return 7; } // LD (HL),D
unsigned Z80::op73() { setReg(6, getReg(3)); return 7; } // LD (HL),E
unsigned Z80::op74() { setReg(6, getReg(4)); return 7; } // LD (HL),H
unsigned Z80::op75() { setReg(6, getReg(5)); return 7; } // LD (HL),L
// 0x76 is HALT, defined below
unsigned Z80::op77() { setReg(6, getReg(7)); return 7; } // LD (HL),A

unsigned Z80::op78() { setReg(7, getReg(0)); return 4; } // LD A,B
unsigned Z80::op79() { setReg(7, getReg(1)); return 4; } // LD A,C
unsigned Z80::op7A() { setReg(7, getReg(2)); return 4; } // LD A,D
unsigned Z80::op7B() { setReg(7, getReg(3)); return 4; } // LD A,E
unsigned Z80::op7C() { setReg(7, getReg(4)); return 4; } // LD A,H
unsigned Z80::op7D() { setReg(7, getReg(5)); return 4; } // LD A,L
unsigned Z80::op7E() { setReg(7, getReg(6)); return 7; } // LD A,(HL)
unsigned Z80::op7F() { setReg(7, getReg(7)); return 4; } // LD A,A

unsigned Z80::op76() { // HALT
    is_halted = true;
    return 4;
}
