#include "catch.hpp"
#include "devices/trs80m3/keyboard.hpp"
#include "machines/trs80m3/host_keyboard_adapter.hpp"
#include "machines/trs80m3/host_typing.hpp"
#include "machines/trs80m3/typing_chord.hpp"

TEST_CASE("TRS-80 Model III host typing maps host quote text to matrix chord",
          "[machine][trs80m3][fast]") {
    char out = '\0';
    REQUIRE(trs80m3HostCharFromText("\"", out));
    REQUIRE(out == '"');

    Trs80M3Keyboard keyboard;
    Trs80M3HostKeyboardAdapter adapter(keyboard);
    REQUIRE(trs80m3HostPulseText(adapter, "\""));
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(4)) == 0x04);
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x01);
}

TEST_CASE("TRS-80 Model III host typing ignores unsupported host characters",
          "[machine][trs80m3][fast]") {
    char out = '\0';
    REQUIRE_FALSE(trs80m3HostCharFromText("ç", out));

    Trs80M3Keyboard keyboard;
    Trs80M3HostKeyboardAdapter adapter(keyboard);
    REQUIRE_FALSE(trs80m3HostPulseText(adapter, "ç"));
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(0)) == 0x00);
}

TEST_CASE("TRS-80 Model III typing chord maps shifted punctuation", "[machine][trs80m3][fast]") {
    Trs80M3TypingChord chord;
    REQUIRE(trs80m3TypingChordForChar('!', chord));
    REQUIRE(chord.row == 4);
    REQUIRE(chord.bit == 1);
    REQUIRE(chord.shift);

    REQUIRE(trs80m3TypingChordForChar('A', chord));
    REQUIRE(chord.row == 0);
    REQUIRE(chord.bit == 1);
    REQUIRE_FALSE(chord.shift);
}
