#include "catch.hpp"
#include "devices/sinclair/host_keymap.hpp"
#include "devices/sinclair/typing_chord.hpp"

TEST_CASE("Sinclair host keymap maps letters and digits", "[device][sinclair][fast]") {
    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;

    REQUIRE(sinclairHostKeyFromLetter('p', key));
    REQUIRE(key == SinclairKeyboard::Key::P);

    REQUIRE(sinclairHostKeyFromLetter('Z', key));
    REQUIRE(key == SinclairKeyboard::Key::Z);

    REQUIRE(sinclairHostKeyFromDigit('0', key));
    REQUIRE(key == SinclairKeyboard::Key::Digit0);

    REQUIRE_FALSE(sinclairHostKeyFromLetter('!', key));
    REQUIRE_FALSE(sinclairHostKeyFromDigit('a', key));
}

TEST_CASE("Sinclair host keymap builds typing chords for host chars", "[device][sinclair][fast]") {
    SinclairTypingChord chord;

    REQUIRE(sinclairHostTypingChordForHostChar('K', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::K);
    REQUIRE_FALSE(chord.shift);

    REQUIRE(sinclairHostTypingChordForHostChar('.', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::Dot);
    REQUIRE_FALSE(chord.shift);

    REQUIRE(sinclairHostTypingChordForHostChar('"', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::P);
    REQUIRE(chord.shift);
}
