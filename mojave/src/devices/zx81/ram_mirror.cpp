#include "devices/zx81/ram_mirror.hpp"

Zx81RamMirror::Zx81RamMirror(Memory& ram) : ram_(ram) {}

uint8_t Zx81RamMirror::read(uint16_t offset) {
    const uint8_t value = ram_.read(static_cast<uint16_t>(offset & kRamMask));
    if (value == Zx81DisplayFile::kNewline)
        return Zx81DisplayFile::kNewline;
    return 0x00;
}

void Zx81RamMirror::write(uint16_t offset, uint8_t value) {
    ram_.write(static_cast<uint16_t>(offset & kRamMask), value);
}
