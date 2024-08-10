#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "devices/sinclair/host_keymap.hpp"
#include "devices/sinclair/host_typing.hpp"
#include "devices/sinclair/typing_chord.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("Sinclair ZX-81 host keymap maps PC letters and digits to matrix keys",
          "[machine][zx81][fast]") {
    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;

    REQUIRE(sinclairHostKeyFromLetter('P', key));
    REQUIRE(key == SinclairKeyboard::Key::P);

    REQUIRE(sinclairHostKeyFromDigit('3', key));
    REQUIRE(key == SinclairKeyboard::Key::Digit3);
}

TEST_CASE("Sinclair ZX-81 host typing maps shifted host symbols to ZX chords",
          "[machine][zx81][fast]") {
    SinclairTypingChord chord;
    REQUIRE(sinclairTypingChordForChar('"', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::P);
    REQUIRE(chord.shift);

    REQUIRE(sinclairTypingChordForChar('<', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::N);
    REQUIRE(chord.shift);

    REQUIRE(sinclairTypingChordForChar('+', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::K);
    REQUIRE(chord.shift);
}

TEST_CASE("Sinclair ZX-81 host typing ignores unsupported host characters",
          "[machine][zx81][fast]") {
    char out = '\0';
    REQUIRE_FALSE(sinclairHostCharFromText("£", out));

    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);
    REQUIRE_FALSE(sinclairHostPulseText(adapter, "£"));
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(0)) == 0xFF);
}

TEST_CASE("Sinclair ZX-81 host keyboard holds a released matrix key briefly",
          "[machine][zx81][fast]") {
    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);

    REQUIRE(adapter.hostKeyDown(SinclairKeyboard::Key::P));
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(5)) == 0xFE);

    adapter.hostKeyUp(SinclairKeyboard::Key::P);
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(5)) == 0xFE);

    for (uint8_t i = 0; i < SinclairHostKeyboardAdapter::kReleaseHoldFrames; ++i)
        adapter.tick();

    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(5)) == 0xFF);
}

TEST_CASE("Sinclair ZX-81 host keyboard pulses rubout as SHIFT+0", "[machine][zx81][fast]") {
    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);

    adapter.pulseKey(SinclairKeyboard::Key::Digit0, true);
    REQUIRE(keyboard.keyDown(0, 0)); // Shift
    REQUIRE(keyboard.keyDown(4, 0)); // Digit0
    REQUIRE(adapter.hasPendingInput());
}
