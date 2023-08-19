#include "catch.hpp"
#include "devices/trs80m3/keyboard.hpp"

TEST_CASE("TRS-80 Model III keyboard maps CONTROL and CAPS LOCK on row 0x3880",
          "[devices][trs80m3][fast]") {
    Trs80M3Keyboard keyboard;

    REQUIRE(Trs80M3Keyboard::rowAddress(7) == 0x3880);

    keyboard.pressSpecialKey(Trs80M3Keyboard::SpecialKey::Control);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x02);
    keyboard.releaseSpecialKey(Trs80M3Keyboard::SpecialKey::Control);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x00);

    keyboard.pressSpecialKey(Trs80M3Keyboard::SpecialKey::CapsLock);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x04);
    keyboard.releaseSpecialKey(Trs80M3Keyboard::SpecialKey::CapsLock);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x00);
}

TEST_CASE("TRS-80 Model III keyboard keeps SHIFT and CONTROL as distinct bits",
          "[devices][trs80m3][fast]") {
    Trs80M3Keyboard keyboard;

    keyboard.pressSpecialKey(Trs80M3Keyboard::SpecialKey::Shift);
    keyboard.pressSpecialKey(Trs80M3Keyboard::SpecialKey::Control);
    keyboard.pressSpecialKey(Trs80M3Keyboard::SpecialKey::CapsLock);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x07);

    keyboard.releaseSpecialKey(Trs80M3Keyboard::SpecialKey::Control);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x05);
}

TEST_CASE("TRS-80 Model III keyboard reuses Model I letter and arrow mapping",
          "[devices][trs80m3][fast]") {
    Trs80M3Keyboard keyboard;

    keyboard.pressNamedKey('A');
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(0)) == 0x02);

    keyboard.pressSpecialKey(Trs80M3Keyboard::SpecialKey::Up);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(6)) == 0x08);
}
