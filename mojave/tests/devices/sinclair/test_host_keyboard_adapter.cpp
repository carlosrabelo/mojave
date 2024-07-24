#include "catch.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "devices/sinclair/host_input.hpp"
#include "devices/sinclair/host_typing.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/keyboard_lookup.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("Sinclair host keyboard adapter holds a released matrix key", "[device][sinclair][fast]") {
    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);

    REQUIRE(adapter.hostKeyDown(SinclairKeyboard::Key::A));
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(1)) == 0xFE);

    adapter.hostKeyUp(SinclairKeyboard::Key::A);
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(1)) == 0xFE);

    for (uint8_t i = 1; i < SinclairHostKeyboardAdapter::kReleaseHoldFrames; ++i) {
        adapter.tick();
        REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(1)) == 0xFE);
    }

    adapter.tick();
    REQUIRE(keyboard.readPort(SinclairKeyboard::rowPort(1)) == 0xFF);
}

TEST_CASE("Sinclair host keyboard adapter pulses Shift+key then releases",
          "[device][sinclair][fast]") {
    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);

    adapter.pulseKey(SinclairKeyboard::Key::Digit0, true);
    REQUIRE(adapter.hasPendingInput());
    REQUIRE(keyboard.keyDown(0, 0)); // Shift
    REQUIRE(keyboard.keyDown(4, 0)); // Digit0

    for (uint8_t i = 0; i < SinclairHostKeyboardAdapter::kPulseFrames; ++i)
        adapter.tick();

    REQUIRE_FALSE(keyboard.keyDown(4, 0));
    REQUIRE_FALSE(keyboard.keyDown(0, 0));
}

TEST_CASE("Sinclair host input helpers sync shift and rubout", "[device][sinclair][fast]") {
    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);

    sinclairHostSyncShift(adapter, true);
    REQUIRE(keyboard.keyDown(0, 0));
    sinclairHostSyncShift(adapter, false);
    for (uint8_t i = 0; i < SinclairHostKeyboardAdapter::kReleaseHoldFrames; ++i)
        adapter.tick();
    REQUIRE_FALSE(keyboard.keyDown(0, 0));

    sinclairHostPulseRubout(adapter);
    REQUIRE(adapter.hasPendingInput());
    REQUIRE(keyboard.keyDown(0, 0));
    REQUIRE(keyboard.keyDown(4, 0));
}

TEST_CASE("Sinclair host typing rejects unsupported UTF-8", "[device][sinclair][fast]") {
    char out = '\0';
    REQUIRE_FALSE(sinclairHostCharFromText("£", out));

    SinclairKeyboard keyboard;
    SinclairHostKeyboardAdapter adapter(keyboard);
    REQUIRE_FALSE(sinclairHostPulseText(adapter, "£"));
}

TEST_CASE("Sinclair keyboard lookup finds matrix on ZX-80 machine", "[device][sinclair][fast]") {
    auto machine = createZx80Machine();
    SinclairKeyboard* keyboard = findSinclairKeyboard(*machine);
    REQUIRE(keyboard != nullptr);
    keyboard->pressKey(SinclairKeyboard::Key::Space);
    REQUIRE(machine->bus().readPort(SinclairKeyboard::rowPort(7)) == 0xFE);
}
