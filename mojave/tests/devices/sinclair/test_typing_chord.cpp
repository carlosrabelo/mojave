#include "catch.hpp"
#include "devices/sinclair/typing_chord.hpp"

TEST_CASE("Sinclair typing chord maps letters and digits unshifted", "[device][sinclair][fast]") {
    SinclairTypingChord chord;

    REQUIRE(sinclairTypingChordForChar('a', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::A);
    REQUIRE_FALSE(chord.shift);

    REQUIRE(sinclairTypingChordForChar('7', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::Digit7);
    REQUIRE_FALSE(chord.shift);

    REQUIRE(sinclairTypingChordForChar(' ', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::Space);
}

TEST_CASE("Sinclair typing chord maps shift legends", "[device][sinclair][fast]") {
    SinclairTypingChord chord;

    REQUIRE(sinclairTypingChordForChar('"', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::P);
    REQUIRE(chord.shift);

    REQUIRE(sinclairTypingChordForChar('?', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::C);
    REQUIRE(chord.shift);

    REQUIRE(sinclairTypingChordForChar('<', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::N);
    REQUIRE(chord.shift);
}

TEST_CASE("Sinclair typing chord letter-mode hint", "[device][sinclair][fast]") {
    SinclairTypingChord letter;
    letter.key = SinclairKeyboard::Key::P;
    letter.shift = false;
    REQUIRE(sinclairTypingChordNeedsLetterMode(letter));

    SinclairTypingChord shifted;
    shifted.key = SinclairKeyboard::Key::P;
    shifted.shift = true;
    REQUIRE_FALSE(sinclairTypingChordNeedsLetterMode(shifted));

    SinclairTypingChord digit;
    digit.key = SinclairKeyboard::Key::Digit0;
    digit.shift = false;
    REQUIRE_FALSE(sinclairTypingChordNeedsLetterMode(digit));
}

TEST_CASE("Sinclair typing chord rejects unsupported characters", "[device][sinclair][fast]") {
    SinclairTypingChord chord;
    REQUIRE_FALSE(sinclairTypingChordForChar('#', chord));
    REQUIRE_FALSE(sinclairTypingChordForChar('\n', chord));
}
