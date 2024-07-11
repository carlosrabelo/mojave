#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "machines/zx80/keyboard_scan.hpp"

TEST_CASE("ZX-80 ROM keyboard scan matches documented matrix positions", "[machine][zx80][fast]") {
    SinclairKeyboard keyboard;

    keyboard.releaseAll();
    REQUIRE(zx80SimulateRomKeyboardScan(keyboard) == 0xFFFF);

    keyboard.pressKey(SinclairKeyboard::Key::S);
    REQUIRE(zx80SimulateRomKeyboardScan(keyboard) == 0xFBFD);
    keyboard.releaseKey(SinclairKeyboard::Key::S);

    keyboard.pressKey(SinclairKeyboard::Key::P);
    REQUIRE(zx80SimulateRomKeyboardScan(keyboard) == 0xFDDF);
    keyboard.releaseKey(SinclairKeyboard::Key::P);

    keyboard.pressKey(SinclairKeyboard::Key::Shift);
    keyboard.pressKey(SinclairKeyboard::Key::P);
    const uint16_t shift_p_scan = zx80SimulateRomKeyboardScan(keyboard);
    REQUIRE(shift_p_scan == 0xFCDF);
    keyboard.releaseKey(SinclairKeyboard::Key::P);
    keyboard.releaseKey(SinclairKeyboard::Key::Shift);
}
