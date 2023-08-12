#ifndef MOJAVE_TRS80M3_FONT_HPP
#define MOJAVE_TRS80M3_FONT_HPP

#include <cstdint>

namespace trs80m3 {

inline constexpr uint8_t kGlyphRows = 12;
inline constexpr uint8_t kGlyphWidth = 8;

// Returns an 8-bit glyph row (bit 7 = left pixel) for Model III text and symbols.
uint8_t textGlyphRow(uint8_t ch, uint8_t row);

} // namespace trs80m3

#endif
