#include "catch.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/trs80m1l1/host_keyboard_adapter.hpp"
#include "machines/trs80m1l1/host_typing.hpp"
#include "machines/trs80m1l1/typing_chord.hpp"

TEST_CASE("TRS-80 Model I Level I host typing maps host quote text to matrix chord",
          "[machine][trs80m1l1][fast]") {
    char out = '\0';
    REQUIRE(trs80m1l1HostCharFromText("\"", out));
    REQUIRE(out == '"');

    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);
    REQUIRE(trs80m1l1HostPulseText(adapter, "\""));
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(4)) == 0x04);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x01);
}

TEST_CASE("TRS-80 Model I Level I host typing ignores unsupported host characters",
          "[machine][trs80m1l1][fast]") {
    char out = '\0';
    REQUIRE_FALSE(trs80m1l1HostCharFromText("ç", out));

    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);
    REQUIRE_FALSE(trs80m1l1HostPulseText(adapter, "ç"));
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I typing chord maps shifted punctuation", "[machine][trs80m1l1][fast]") {
    Trs80M1L1TypingChord chord;
    REQUIRE(trs80m1l1TypingChordForChar('!', chord));
    REQUIRE(chord.row == 4);
    REQUIRE(chord.bit == 1);
    REQUIRE(chord.shift);

    REQUIRE(trs80m1l1TypingChordForChar('A', chord));
    REQUIRE(chord.row == 0);
    REQUIRE(chord.bit == 1);
    REQUIRE_FALSE(chord.shift);
}
