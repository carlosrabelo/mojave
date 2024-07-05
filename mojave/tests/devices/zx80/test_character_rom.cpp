#include <array>
#include <cstdint>
#include <vector>
#include "catch.hpp"
#include "devices/zx80/character_rom.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"

namespace {

constexpr std::array<uint8_t, 8> kSpaceGlyph{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// CHR$(29) is the digit '1' in the ZX80 charset.
constexpr std::array<uint8_t, 8> kDigit1Glyph{0x00, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1E, 0x00};

// CHR$(38) is the letter 'A' in the ZX80 charset.
constexpr std::array<uint8_t, 8> kLetterAGlyph{0x00, 0x3E, 0x41, 0x41, 0x7F, 0x41, 0x41, 0x00};

Zx80CharacterRom::ReadFn readerFor(std::vector<uint8_t>& rom) {
    return [&rom](uint16_t address) -> uint8_t {
        if (address < Zx80CharacterRom::kCharsetBase)
            return 0xFF;
        const uint16_t offset = static_cast<uint16_t>(address - Zx80CharacterRom::kCharsetBase);
        if (offset >= rom.size())
            return 0xFF;
        return rom[offset];
    };
}

void writeGlyph(std::vector<uint8_t>& rom, uint8_t code, const std::array<uint8_t, 8>& glyph) {
    const uint16_t offset = static_cast<uint16_t>(Zx80CharacterRom::glyphIndex(code) * Zx80CharacterRom::kGlyphRows);
    for (uint8_t row = 0; row < Zx80CharacterRom::kGlyphRows; ++row)
        rom[offset + row] = glyph[row];
}

} // namespace

TEST_CASE("ZX-80 character ROM constants", "[device][zx80][fast]") {
    REQUIRE(Zx80CharacterRom::kCharsetBase == 0x0E00);
    REQUIRE(Zx80CharacterRom::kCharsetEndExclusive == 0x1000);
    REQUIRE(Zx80CharacterRom::kIRegister == 0x0E);
    REQUIRE(Zx80CharacterRom::kGlyphCount == 64u);
    REQUIRE(Zx80CharacterRom::kGlyphRows == 8u);
    REQUIRE(Zx80CharacterRom::kCharsetBase == Zx80PresetContract::charset_rom_start);
    REQUIRE(Zx80CharacterRom::kCharsetEndExclusive == Zx80PresetContract::charset_rom_end_exclusive);
    REQUIRE(Zx80CharacterRom::kIRegister == Zx80PresetContract::charset_i_register);
}

TEST_CASE("ZX-80 character ROM maps code to glyph index and address", "[device][zx80][fast]") {
    REQUIRE(Zx80CharacterRom::glyphIndex(0) == 0);
    REQUIRE(Zx80CharacterRom::glyphIndex(29) == 29);
    REQUIRE(Zx80CharacterRom::glyphIndex(38) == 38);
    REQUIRE(Zx80CharacterRom::glyphIndex(0x7F) == 0x3F);
    REQUIRE(Zx80CharacterRom::glyphIndex(0xC5) == 0x05);

    REQUIRE(Zx80CharacterRom::glyphRowAddress(0, 0) == 0x0E00);
    REQUIRE(Zx80CharacterRom::glyphRowAddress(29, 3) == 0x0EE8 + 3);
    REQUIRE(Zx80CharacterRom::glyphRowAddress(38, 1) == 0x0F31);
}

TEST_CASE("ZX-80 character ROM reads space glyph from ROM", "[device][zx80][fast]") {
    std::vector<uint8_t> rom(512, 0xFF);
    writeGlyph(rom, 0, kSpaceGlyph);

    const auto glyph = Zx80CharacterRom::readGlyph(readerFor(rom), 0);
    REQUIRE(glyph == kSpaceGlyph);
}

TEST_CASE("ZX-80 character ROM reads digit 1 and letter A glyphs", "[device][zx80][fast]") {
    std::vector<uint8_t> rom(512, 0xFF);
    writeGlyph(rom, 29, kDigit1Glyph);
    writeGlyph(rom, 38, kLetterAGlyph);

    REQUIRE(Zx80CharacterRom::readGlyph(readerFor(rom), 29) == kDigit1Glyph);
    REQUIRE(Zx80CharacterRom::readGlyph(readerFor(rom), 38) == kLetterAGlyph);
}

TEST_CASE("ZX-80 character ROM applies invert attribute on bit 6", "[device][zx80][fast]") {
    std::vector<uint8_t> rom(512, 0xFF);
    writeGlyph(rom, 29, kDigit1Glyph);

    REQUIRE(Zx80CharacterRom::isInverted(29) == false);
    REQUIRE(Zx80CharacterRom::isInverted(29 | Zx80CharacterRom::kInvertBit) == true);

    const uint8_t normal = Zx80CharacterRom::readRow(readerFor(rom), 29, 1);
    const uint8_t inverted = Zx80CharacterRom::readRow(readerFor(rom), 29 | Zx80CharacterRom::kInvertBit, 1);
    REQUIRE(normal == 0x0C);
    REQUIRE(inverted == static_cast<uint8_t>(~0x0C));
}

TEST_CASE("ZX-80 character ROM returns 0xFF for out-of-range row", "[device][zx80][fast]") {
    std::vector<uint8_t> rom(512, 0x00);
  REQUIRE(Zx80CharacterRom::readRow(readerFor(rom), 0, 8) == 0xFF);
}

TEST_CASE("ZX-80 machine ROM region covers charset addresses", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    REQUIRE(machine->bus().read(Zx80CharacterRom::kCharsetBase) == 0x00);
    REQUIRE(machine->bus().read(static_cast<uint16_t>(Zx80CharacterRom::kCharsetEndExclusive - 1)) == 0x00);
    REQUIRE(machine->bus().read(Zx80PresetContract::rom_end_exclusive) == 0xFF);
}
