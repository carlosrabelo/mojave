#include <cstdint>
#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"

TEST_CASE("Sinclair keyboard constants", "[device][sinclair][fast]") {
    REQUIRE(SinclairKeyboard::kPortLowByte == 0x00FE);
    REQUIRE(SinclairKeyboard::kRowCount == 8);
    REQUIRE(SinclairKeyboard::kBitsPerRow == 5);
    REQUIRE(SinclairKeyboard::rowPort(0) == 0xFEFE);
    REQUIRE(SinclairKeyboard::rowPort(7) == 0x7FFE);
}

TEST_CASE("Sinclair keyboard row select comes from port high byte", "[device][sinclair][fast]") {
    REQUIRE(SinclairKeyboard::rowFromPort(0xFEFE) == 0);
    REQUIRE(SinclairKeyboard::rowFromPort(0xFDFE) == 1);
    REQUIRE(SinclairKeyboard::rowFromPort(0xFBFE) == 2);
    REQUIRE(SinclairKeyboard::rowFromPort(0xF7FE) == 3);
    REQUIRE(SinclairKeyboard::rowFromPort(0xEFFE) == 4);
    REQUIRE(SinclairKeyboard::rowFromPort(0xDFFE) == 5);
    REQUIRE(SinclairKeyboard::rowFromPort(0xBFFE) == 6);
    REQUIRE(SinclairKeyboard::rowFromPort(0x7FFE) == 7);

    REQUIRE_FALSE(SinclairKeyboard::rowFromPort(0xFFFF).has_value());
    REQUIRE_FALSE(SinclairKeyboard::rowFromPort(0xFEFE | 0x0001).has_value());
    REQUIRE_FALSE(SinclairKeyboard::rowFromPort(0x00FE).has_value());
}

TEST_CASE("Sinclair keyboard reads active-low key data on port", "[device][sinclair][fast]") {
    SinclairKeyboard keyboard;

    REQUIRE(keyboard.readPort(0xFEFE) == 0xFF);

    keyboard.pressKey(SinclairKeyboard::Key::Shift);
    REQUIRE(keyboard.readPort(0xFEFE) == 0xFE);
    keyboard.releaseKey(SinclairKeyboard::Key::Shift);
    REQUIRE(keyboard.readPort(0xFEFE) == 0xFF);

    keyboard.pressKey(SinclairKeyboard::Key::A);
    REQUIRE(keyboard.readPort(0xFDFE) == 0xFE);
    keyboard.releaseKey(SinclairKeyboard::Key::A);

    keyboard.pressKey(SinclairKeyboard::Key::Q);
    REQUIRE(keyboard.readPort(0xFBFE) == 0xFE);
    keyboard.releaseKey(SinclairKeyboard::Key::Q);

    keyboard.pressKey(SinclairKeyboard::Key::Space);
    REQUIRE(keyboard.readPort(0x7FFE) == 0xFE);
    keyboard.releaseKey(SinclairKeyboard::Key::Space);
}

TEST_CASE("Sinclair keyboard scans each row independently", "[device][sinclair][fast]") {
    SinclairKeyboard keyboard;
    const SinclairKeyboard::Key per_row[] = {
        SinclairKeyboard::Key::Shift,
        SinclairKeyboard::Key::A,
        SinclairKeyboard::Key::Q,
        SinclairKeyboard::Key::Digit1,
        SinclairKeyboard::Key::Digit0,
        SinclairKeyboard::Key::P,
        SinclairKeyboard::Key::Enter,
        SinclairKeyboard::Key::Space,
    };

    for (uint8_t row = 0; row < SinclairKeyboard::kRowCount; ++row) {
        keyboard.releaseAll();
        keyboard.pressKey(per_row[row]);
        REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(row)) == 0xFE);
        for (uint8_t other = 0; other < SinclairKeyboard::kRowCount; ++other) {
            if (other == row)
                continue;
            REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(other)) == 0xFF);
        }
    }
}
