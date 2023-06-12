#include "cpus/m6502.hpp"

// --- JUMPS ---

unsigned M6502::op4C() { // JMP abs
    regs_.pc = addrAbsolute();
    return 3;
}

unsigned M6502::op6C() { // JMP (ind)
    regs_.pc = addrIndirect();
    return 5;
}

unsigned M6502::op20() { // JSR abs
    uint16_t target = read16(regs_.pc);
    regs_.pc += 2;
    uint16_t ret_pc = regs_.pc - 1; // last byte of JSR instruction
    push8(static_cast<uint8_t>(ret_pc >> 8));
    push8(static_cast<uint8_t>(ret_pc & 0xFF));
    regs_.pc = target;
    return 6;
}

unsigned M6502::op60() { // RTS
    uint16_t ret_pc = pop8();
    ret_pc |= (static_cast<uint16_t>(pop8()) << 8);
    regs_.pc = ret_pc + 1;
    return 6;
}

// --- BRANCHES ---

unsigned M6502::op10() { // BPL rel
    return aluBranch(!getFlagN());
}

unsigned M6502::op30() { // BMI rel
    return aluBranch(getFlagN());
}

unsigned M6502::op50() { // BVC rel
    return aluBranch(!getFlagV());
}

unsigned M6502::op70() { // BVS rel
    return aluBranch(getFlagV());
}

unsigned M6502::op90() { // BCC rel
    return aluBranch(!getFlagC());
}

unsigned M6502::opB0() { // BCS rel
    return aluBranch(getFlagC());
}

unsigned M6502::opD0() { // BNE rel
    return aluBranch(!getFlagZ());
}

unsigned M6502::opF0() { // BEQ rel
    return aluBranch(getFlagZ());
}
