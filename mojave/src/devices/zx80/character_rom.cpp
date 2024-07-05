#include "devices/zx80/character_rom.hpp"

uint8_t Zx80CharacterRom::glyphIndex(uint8_t code) {
    return static_cast<uint8_t>(code & kGlyphIndexMask);
}

bool Zx80CharacterRom::isInverted(uint8_t code) {
    return (code & kInvertBit) != 0;
}

uint16_t Zx80CharacterRom::glyphRowAddress(uint8_t code, uint8_t row) {
    return static_cast<uint16_t>(kCharsetBase + static_cast<uint16_t>(glyphIndex(code)) * kGlyphRows + row);
}

uint8_t Zx80CharacterRom::readRow(ReadFn read, uint8_t code, uint8_t row) {
    if (!read || row >= kGlyphRows)
        return 0xFF;

    const uint16_t address = glyphRowAddress(code, row);
    if (address < kCharsetBase || address >= kCharsetEndExclusive)
        return 0xFF;

    uint8_t pixels = read(address);
    if (isInverted(code))
        pixels = static_cast<uint8_t>(~pixels);
    return pixels;
}

std::array<uint8_t, Zx80CharacterRom::kGlyphRows> Zx80CharacterRom::readGlyph(ReadFn read, uint8_t code) {
    std::array<uint8_t, kGlyphRows> rows{};
    for (uint8_t row = 0; row < kGlyphRows; ++row)
        rows[row] = readRow(read, code, row);
    return rows;
}
