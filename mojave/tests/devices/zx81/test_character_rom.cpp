#include <array>
#include <cstdint>
#include <vector>
#include "catch.hpp"
#include "devices/zx81/character_rom.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"

namespace {

constexpr std::array<uint8_t, 8> kSpaceGlyph{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// CHR$(29) is the digit '1' in the ZX-81 charset.
constexpr std::array<uint8_t, 8> kDigit1Glyph{0x00, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1E, 0x00};

// CHR$(38) is the letter 'A' in the ZX-81 charset.
constexpr std::array<uint8_t, 8> kLetterAGlyph{0x00, 0x3E, 0x41, 0x41, 0x7F, 0x41, 0x41, 0x00};

Zx81CharacterRom::ReadFn readerFor(std::vector<uint8_t>& rom) {
    return [&rom](uint16_t address) -> uint8_t {
        if (address < Zx81CharacterRom::kCharsetBase)
            return 0xFF;
        const uint16_t offset = static_cast<uint16_t>(address - Zx81CharacterRom::kCharsetBase);
        if (offset >= rom.size())
            return 0xFF;
        return rom[offset];
    };
}

void writeGlyph(std::vector<uint8_t>& rom, uint8_t code, const std::array<uint8_t, 8>& glyph) {
    const uint16_t offset = static_cast<uint16_t>(Zx81CharacterRom::glyphIndex(code) * Zx81CharacterRom::kGlyphRows);
    for (uint8_t row = 0; row < Zx81CharacterRom::kGlyphRows; ++row)
        rom[offset + row] = glyph[row];
}

} // namespace

TEST_CASE("ZX-81 character ROM constants", "[device][zx81][fast]") {
    REQUIRE(Zx81CharacterRom::kCharsetBase == 0x1E00);
    REQUIRE(Zx81CharacterRom::kCharsetEndExclusive == 0x2000);
    REQUIRE(Zx81CharacterRom::kIRegister == 0x1E);
    REQUIRE(Zx81CharacterRom::kGlyphCount == 64u);
    REQUIRE(Zx81CharacterRom::kGlyphRows == 8u);
    REQUIRE(Zx81CharacterRom::kInvertBit == 0x80);
    REQUIRE(Zx81CharacterRom::kCharsetBase == Zx81PresetContract::charset_rom_start);
    REQUIRE(Zx81CharacterRom::kCharsetEndExclusive == Zx81PresetContract::charset_rom_end_exclusive);
    REQUIRE(Zx81CharacterRom::kIRegister == Zx81PresetContract::charset_i_register);
}

TEST_CASE("ZX-81 character ROM maps code to glyph index and address", "[device][zx81][fast]") {
    REQUIRE(Zx81CharacterRom::glyphIndex(0) == 0);
    REQUIRE(Zx81CharacterRom::glyphIndex(29) == 29);
    REQUIRE(Zx81CharacterRom::glyphIndex(38) == 38);
    REQUIRE(Zx81CharacterRom::glyphIndex(0x7F) == 0x3F);
    REQUIRE(Zx81CharacterRom::glyphIndex(0xC5) == 0x05);

    REQUIRE(Zx81CharacterRom::glyphRowAddress(0, 0) == 0x1E00);
    REQUIRE(Zx81CharacterRom::glyphRowAddress(29, 3) == 0x1EE8 + 3);
    REQUIRE(Zx81CharacterRom::glyphRowAddress(38, 1) == 0x1F31);
}

TEST_CASE("ZX-81 character ROM reads space glyph from ROM", "[device][zx81][fast]") {
    std::vector<uint8_t> rom(512, 0xFF);
    writeGlyph(rom, 0, kSpaceGlyph);

    const auto glyph = Zx81CharacterRom::readGlyph(readerFor(rom), 0);
    REQUIRE(glyph == kSpaceGlyph);
}

TEST_CASE("ZX-81 character ROM reads digit 1 and letter A glyphs", "[device][zx81][fast]") {
    std::vector<uint8_t> rom(512, 0xFF);
    writeGlyph(rom, 29, kDigit1Glyph);
    writeGlyph(rom, 38, kLetterAGlyph);

    REQUIRE(Zx81CharacterRom::readGlyph(readerFor(rom), 29) == kDigit1Glyph);
    REQUIRE(Zx81CharacterRom::readGlyph(readerFor(rom), 38) == kLetterAGlyph);
}

TEST_CASE("ZX-81 character ROM applies invert attribute on bit 7", "[device][zx81][fast]") {
    std::vector<uint8_t> rom(512, 0xFF);
    writeGlyph(rom, 29, kDigit1Glyph);

    REQUIRE(Zx81CharacterRom::isInverted(29) == false);
    REQUIRE(Zx81CharacterRom::isInverted(29 | Zx81CharacterRom::kInvertBit) == true);
    REQUIRE(Zx81CharacterRom::isInverted(29 | 0x40) == false);

    const uint8_t normal = Zx81CharacterRom::readRow(readerFor(rom), 29, 1);
    const uint8_t inverted = Zx81CharacterRom::readRow(readerFor(rom), 29 | Zx81CharacterRom::kInvertBit, 1);
    REQUIRE(normal == 0x0C);
    REQUIRE(inverted == static_cast<uint8_t>(~0x0C));
}

TEST_CASE("ZX-81 character ROM returns 0xFF for out-of-range row", "[device][zx81][fast]") {
    std::vector<uint8_t> rom(512, 0x00);
    REQUIRE(Zx81CharacterRom::readRow(readerFor(rom), 0, 8) == 0xFF);
}

TEST_CASE("ZX-81 machine ROM region covers charset addresses", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    REQUIRE(machine->bus().read(Zx81CharacterRom::kCharsetBase) == 0x00);
    REQUIRE(machine->bus().read(static_cast<uint16_t>(Zx81CharacterRom::kCharsetEndExclusive - 1)) == 0x00);
    REQUIRE(machine->bus().read(Zx81PresetContract::rom_end_exclusive) == 0xFF);
}
