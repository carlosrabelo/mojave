#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "devices/sinclair/host_keymap.hpp"
#include "devices/sinclair/host_typing.hpp"
#include "devices/sinclair/typing_chord.hpp"
#include "devices/sinclair/keyboard_lookup.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("Sinclair ZX-80 host keymap maps PC letters and digits to matrix keys",
          "[machine][zx80][fast]") {
    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;

    REQUIRE(sinclairHostKeyFromLetter('P', key));
    REQUIRE(key == SinclairKeyboard::Key::P);

    REQUIRE(sinclairHostKeyFromDigit('3', key));
    REQUIRE(key == SinclairKeyboard::Key::Digit3);
}

TEST_CASE("Sinclair ZX-80 host typing maps shifted host symbols to ZX chords",
          "[machine][zx80][fast]") {
    SinclairTypingChord chord;
    REQUIRE(sinclairTypingChordForChar('"', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::P);
    REQUIRE(chord.shift);

    REQUIRE(sinclairTypingChordForChar('<', chord));
    REQUIRE(chord.key == SinclairKeyboard::Key::N);
    REQUIRE(chord.shift);
}

TEST_CASE("Sinclair ZX-80 host typing ignores unsupported host characters",
          "[machine][zx80][fast]") {
    char out = '\0';
    REQUIRE_FALSE(sinclairHostCharFromText("£", out));

    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);
    REQUIRE_FALSE(sinclairHostPulseText(adapter, "£"));
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(0)) == 0xFF);
}

TEST_CASE("Sinclair ZX-80 host keyboard holds a released matrix key briefly",
          "[machine][zx80][fast]") {
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

TEST_CASE("Sinclair ZX-80 host keyboard does not alter FLAGS for matrix keys",
          "[machine][zx80][fast]") {
    auto machine = createZx80Machine();
    SinclairKeyboard* keyboard = findSinclairKeyboard(*machine);
    REQUIRE(keyboard != nullptr);
    SinclairHostKeyboardAdapter adapter(*keyboard);

    machine->bus().write(Zx80PresetContract::flags_address, Zx80PresetContract::flags_k_mode_mask);
    REQUIRE(adapter.hostKeyDown(SinclairKeyboard::Key::P));
    REQUIRE((machine->bus().read(Zx80PresetContract::flags_address) & Zx80PresetContract::flags_k_mode_mask) != 0);
}
