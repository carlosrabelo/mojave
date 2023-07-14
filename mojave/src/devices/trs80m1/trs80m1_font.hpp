#ifndef MOJAVE_TRS80M1_FONT_HPP
#define MOJAVE_TRS80M1_FONT_HPP

#include <cstdint>

namespace trs80m1 {

inline constexpr uint8_t kGlyphRows = 12;
inline constexpr uint8_t kGlyphWidth = 6;

// Returns a 6-bit glyph row (bit 5 = left pixel) for uppercase text and symbols.
uint8_t textGlyphRow(uint8_t ch, uint8_t row);

} // namespace trs80m1

#endif
