#include "devices/trs80m3/trs80m3_font.hpp"
#include "catch.hpp"

TEST_CASE("TRS-80 Model III font renders distinct lowercase glyphs", "[devices][trs80m3][fast]") {
    const uint8_t upper_a = trs80m3::textGlyphRow('A', 4);
    const uint8_t lower_a = trs80m3::textGlyphRow('a', 4);
    REQUIRE(upper_a != lower_a);

    const uint8_t upper_z = trs80m3::textGlyphRow('Z', 6);
    const uint8_t lower_z = trs80m3::textGlyphRow('z', 6);
    REQUIRE(upper_z != lower_z);
}

TEST_CASE("TRS-80 Model III font folds Model I letter codes", "[devices][trs80m3][fast]") {
    REQUIRE(trs80m3::textGlyphRow(0x01, 4) == trs80m3::textGlyphRow('A', 4));
    REQUIRE(trs80m3::textGlyphRow(0x00, 2) == trs80m3::textGlyphRow('@', 2));
}

TEST_CASE("TRS-80 Model III font uses 8-bit glyph rows", "[devices][trs80m3][fast]") {
    REQUIRE(trs80m3::kGlyphWidth == 8u);
    REQUIRE(trs80m3::kGlyphRows == 12u);
    REQUIRE(trs80m3::textGlyphRow('A', 2) == 0x30);
}
