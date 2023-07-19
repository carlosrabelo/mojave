#include "catch.hpp"
#include "devices/trs80m1/keyboard.hpp"

TEST_CASE("TRS-80 Model I Level I keyboard matrix row decode", "[devices][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;

    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);

    keyboard.pressNamedKey('A');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);
    keyboard.releaseNamedKey('A');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I keyboard host input maps letters and shift", "[devices][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;

    keyboard.pressNamedKey('A');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);
    keyboard.releaseNamedKey('A');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);

    keyboard.pressNamedKey('a');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);
    keyboard.releaseNamedKey('a');

    keyboard.pressSpecialKey(Trs80M1Keyboard::SpecialKey::Shift);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x01);
    keyboard.releaseSpecialKey(Trs80M1Keyboard::SpecialKey::Shift);
}

TEST_CASE("TRS-80 Model I Level I keyboard maps special keys without control or caps lock",
          "[devices][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;

    keyboard.pressSpecialKey(Trs80M1Keyboard::SpecialKey::Up);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(6)) == 0x08);

    keyboard.releaseAll();
    keyboard.pressSpecialKey(Trs80M1Keyboard::SpecialKey::Enter);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(6)) == 0x01);

    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x00);
    keyboard.reset();
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(6)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I keyboard OR-combines selected rows", "[devices][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;

    keyboard.pressNamedKey('A');
    keyboard.pressNamedKey('H');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(1)) == 0x01);
    REQUIRE(keyboard.read(static_cast<uint16_t>(Trs80M1Keyboard::rowOffset(0) |
                                                Trs80M1Keyboard::rowOffset(1))) == 0x03);
}

TEST_CASE("TRS-80 Model I Level I keyboard ignores writes", "[devices][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;

    keyboard.write(Trs80M1Keyboard::rowOffset(0), 0xFF);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);
}
