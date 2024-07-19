#include <cstdint>
#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "machines/zx80/zx80_preset.hpp"

TEST_CASE("ZX-80 keyboard contract matches Sinclair matrix", "[device][zx80][fast]") {
    REQUIRE(SinclairKeyboard::kPortLowByte == Zx80PresetContract::keyboard_port_low_byte);
    REQUIRE(SinclairKeyboard::kRowCount == Zx80PresetContract::keyboard_row_count);
    REQUIRE(SinclairKeyboard::kBitsPerRow == Zx80PresetContract::keyboard_bits_per_row);
}
