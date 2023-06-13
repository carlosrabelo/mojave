#include "cpus/m6502.hpp"

// --- INTERRUPTS ---

unsigned M6502::op00() { // BRK
    // BRK pushes PC + 1 (PC already incremented by 1 during fetch of 0x00)
    uint16_t ret_pc = regs_.pc + 1;
    push8(static_cast<uint8_t>(ret_pc >> 8));
    push8(static_cast<uint8_t>(ret_pc & 0xFF));
    push8(regs_.p | 0x30); // B (bit 4) and U (bit 5) set
    setFlagI(true);
    regs_.pc = read16(0xFFFE);
    return 7;
}

unsigned M6502::op40() { // RTI
    regs_.p = (pop8() & ~0x10) | 0x20; // ignore B (bit 4), set U (bit 5)
    uint16_t low = pop8();
    uint16_t high = pop8();
    regs_.pc = (high << 8) | low;
    return 6;
}

// --- FLAG INSTRUCTIONS ---

unsigned M6502::op18() { // CLC
    setFlagC(false);
    return 2;
}

unsigned M6502::op38() { // SEC
    setFlagC(true);
    return 2;
}

unsigned M6502::op58() { // CLI
    setFlagI(false);
    return 2;
}

unsigned M6502::op78() { // SEI
    setFlagI(true);
    return 2;
}

unsigned M6502::opB8() { // CLV
    setFlagV(false);
    return 2;
}

unsigned M6502::opD8() { // CLD
    setFlagD(false);
    return 2;
}

unsigned M6502::opF8() { // SED
    setFlagD(true);
    return 2;
}

unsigned M6502::opEA() { // NOP
    return 2;
}
