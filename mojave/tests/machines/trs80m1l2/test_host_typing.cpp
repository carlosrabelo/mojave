#include "catch.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/trs80m1l1/host_keyboard_adapter.hpp"
#include "machines/trs80m1l1/host_typing.hpp"
#include "machines/trs80m1l1/typing_chord.hpp"
#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("TRS-80 Model I Level II host typing maps host quote text to matrix chord",
          "[machine][trs80m1l2][fast]") {
    char out = '\0';
    REQUIRE(trs80m1l1HostCharFromText("\"", out));
    REQUIRE(out == '"');

    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);
    REQUIRE(trs80m1l1HostPulseText(adapter, "\""));
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(4)) == 0x04);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x01);
}

TEST_CASE("TRS-80 Model I Level II host typing ignores unsupported host characters",
          "[machine][trs80m1l2][fast]") {
    char out = '\0';
    REQUIRE_FALSE(trs80m1l1HostCharFromText("ç", out));

    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);
    REQUIRE_FALSE(trs80m1l1HostPulseText(adapter, "ç"));
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level II typing chord maps shifted punctuation", "[machine][trs80m1l2][fast]") {
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

TEST_CASE("TRS-80 Model I Level II host typing pulses layout text through machine bridge",
          "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    auto bridge = Trs80HostKeyboardBridge::fromMachine(*machine);
    REQUIRE(bridge.has_value());

    REQUIRE(trs80m1l1HostPulseText(bridge->adapter(), "\""));
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(4)) == 0x04);
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(7)) == 0x01);

    for (uint8_t i = 0; i < Trs80M1L1HostKeyboardAdapter::kPulseFrames; ++i)
        bridge->adapter().tick();

    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(4)) == 0x00);
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(7)) == 0x00);
}
