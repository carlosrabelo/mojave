#include <cstdint>
#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "machines/zx81/zx81_preset.hpp"

TEST_CASE("ZX-81 keyboard contract matches Sinclair matrix", "[device][zx81][fast]") {
    REQUIRE(SinclairKeyboard::kPortLowByte == Zx81PresetContract::keyboard_port_low_byte);
    REQUIRE(SinclairKeyboard::kRowCount == Zx81PresetContract::keyboard_row_count);
    REQUIRE(SinclairKeyboard::kBitsPerRow == Zx81PresetContract::keyboard_bits_per_row);
}
