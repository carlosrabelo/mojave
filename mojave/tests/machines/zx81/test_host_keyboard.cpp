#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/zx81/zx81_host_keyboard_bridge.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("Sinclair ZX-81 host keyboard adapter holds a released matrix key briefly",
          "[machine][zx81][fast]") {
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

TEST_CASE("Sinclair ZX-81 host keyboard bridge attaches to machine", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();
    auto bridge = Zx81HostKeyboardBridge::fromMachine(*machine);
    REQUIRE(bridge.has_value());

    REQUIRE(bridge->adapter().hostKeyDown(SinclairKeyboard::Key::H));
    REQUIRE(machine->bus().readPort(SinclairKeyboard::rowPort(6)) == 0xEF);
    bridge->adapter().hostKeyUp(SinclairKeyboard::Key::H);
    for (uint8_t i = 0; i < SinclairHostKeyboardAdapter::kReleaseHoldFrames; ++i)
        bridge->adapter().tick();
    REQUIRE(machine->bus().readPort(SinclairKeyboard::rowPort(6)) == 0xFF);
}
