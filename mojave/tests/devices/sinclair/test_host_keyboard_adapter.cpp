#include "catch.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
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

TEST_CASE("Sinclair keyboard lookup finds matrix on ZX-80 machine", "[device][sinclair][fast]") {
    auto machine = createZx80Machine();
    SinclairKeyboard* keyboard = findSinclairKeyboard(*machine);
    REQUIRE(keyboard != nullptr);
    keyboard->pressKey(SinclairKeyboard::Key::Space);
    REQUIRE(machine->bus().readPort(SinclairKeyboard::rowPort(7)) == 0xFE);
}
