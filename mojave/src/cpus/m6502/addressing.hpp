#ifndef MOJAVE_M6502_ADDRESSING_HPP
#define MOJAVE_M6502_ADDRESSING_HPP

#include "cpus/m6502.hpp"

inline uint16_t M6502::addrImmediate() {
    return regs_.pc++;
}

inline uint16_t M6502::addrAbsolute() {
    uint16_t addr = read16(regs_.pc);
    regs_.pc += 2;
    return addr;
}

inline uint16_t M6502::addrZeroPage() {
    return readByte(regs_.pc++);
}

inline uint16_t M6502::addrAbsoluteX(bool& page_crossed) {
    uint16_t base = read16(regs_.pc);
    regs_.pc += 2;
    uint16_t addr = base + regs_.x;
    page_crossed = (base & 0xFF00) != (addr & 0xFF00);
    return addr;
}

inline uint16_t M6502::addrAbsoluteY(bool& page_crossed) {
    uint16_t base = read16(regs_.pc);
    regs_.pc += 2;
    uint16_t addr = base + regs_.y;
    page_crossed = (base & 0xFF00) != (addr & 0xFF00);
    return addr;
}

inline uint16_t M6502::addrZeroPageX() {
    uint8_t base = readByte(regs_.pc++);
    return static_cast<uint8_t>(base + regs_.x);
}

inline uint16_t M6502::addrZeroPageY() {
    uint8_t base = readByte(regs_.pc++);
    return static_cast<uint8_t>(base + regs_.y);
}

inline uint16_t M6502::addrIndexedIndirect() {
    uint8_t zp_base = readByte(regs_.pc++);
    uint8_t zp_addr = static_cast<uint8_t>(zp_base + regs_.x);
    return read16_zp(zp_addr);
}

inline uint16_t M6502::addrIndirectIndexed(bool& page_crossed) {
    uint8_t zp_addr = readByte(regs_.pc++);
    uint16_t base = read16_zp(zp_addr);
    uint16_t addr = base + regs_.y;
    page_crossed = (base & 0xFF00) != (addr & 0xFF00);
    return addr;
}

inline uint16_t M6502::addrRelative(bool& page_crossed) {
    int8_t offset = static_cast<int8_t>(readByte(regs_.pc++));
    uint16_t base = regs_.pc;
    uint16_t addr = base + offset;
    page_crossed = (base & 0xFF00) != (addr & 0xFF00);
    return addr;
}

inline uint16_t M6502::addrIndirect() {
    uint16_t ptr = read16(regs_.pc);
    regs_.pc += 2;
    
    uint8_t low = readByte(ptr);
    uint8_t high = readByte((ptr & 0xFF00) | static_cast<uint8_t>((ptr & 0xFF) + 1));
    return (high << 8) | low;
}

#endif
