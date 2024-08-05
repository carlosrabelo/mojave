#ifndef MOJAVE_ZX81_RAM_MIRROR_HPP
#define MOJAVE_ZX81_RAM_MIRROR_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/memory.hpp"
#include "devices/zx81/display_file.hpp"

// High-address echo of built-in RAM (0xC000+ → 0x4000+).
// During display generation the ULA feeds the CPU NOPs for character codes
// while NEWLINE (0x76) still executes as HALT so INT can end the scan line.
class Zx81RamMirror : public Device {
public:
    static constexpr uint16_t kMirrorStart = 0xC000;
    static constexpr uint32_t kMirrorEndExclusive = 65536u;
    static constexpr uint16_t kRamMask = 0x03FF;

    explicit Zx81RamMirror(Memory& ram);

    uint8_t read(uint16_t offset) override;
    void write(uint16_t offset, uint8_t value) override;

private:
    Memory& ram_;
};

#endif
