#ifndef MOJAVE_ZX81_CHARACTER_ROM_HPP
#define MOJAVE_ZX81_CHARACTER_ROM_HPP

#include <array>
#include <cstdint>
#include <functional>

class Zx81CharacterRom {
public:
    static constexpr uint16_t kCharsetBase = 0x1E00;
    static constexpr uint16_t kCharsetEndExclusive = 0x2000;
    static constexpr uint8_t kIRegister = 0x1E;
    static constexpr unsigned kGlyphCount = 64;
    static constexpr unsigned kGlyphRows = 8;
    static constexpr uint8_t kInvertBit = 0x80;
    static constexpr uint8_t kGlyphIndexMask = 0x3F;

    using ReadFn = std::function<uint8_t(uint16_t address)>;

    static uint8_t glyphIndex(uint8_t code);
    static bool isInverted(uint8_t code);
    static uint16_t glyphRowAddress(uint8_t code, uint8_t row);
    static uint8_t readRow(ReadFn read, uint8_t code, uint8_t row);
    static std::array<uint8_t, kGlyphRows> readGlyph(ReadFn read, uint8_t code);
};

#endif
