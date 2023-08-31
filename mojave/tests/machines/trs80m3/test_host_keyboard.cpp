#include "catch.hpp"
#include "devices/trs80m3/keyboard.hpp"
#include "machines/trs80m3/host_keyboard_adapter.hpp"
#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/trs80m3/trs80_host_keyboard_bridge.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("TRS-80 Model III host keyboard adapter holds release for several frames",
          "[machine][trs80m3][fast]") {
    Trs80M3Keyboard keyboard;
    Trs80M3HostKeyboardAdapter adapter(keyboard);

    adapter.hostKeyDown('A');
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(0)) == 0x02);

    adapter.hostKeyUp('A');
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(0)) == 0x02);

    for (uint8_t i = 1; i < Trs80M3HostKeyboardAdapter::kReleaseHoldFrames; ++i) {
        adapter.tick();
        REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(0)) == 0x02);
    }

    adapter.tick();
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(0)) == 0x00);
}

TEST_CASE("TRS-80 Model III host keyboard adapter maps CONTROL and CAPS LOCK",
          "[machine][trs80m3][fast]") {
    Trs80M3Keyboard keyboard;
    Trs80M3HostKeyboardAdapter adapter(keyboard);

    REQUIRE(adapter.hostSpecialDown(Trs80M3Keyboard::SpecialKey::Control));
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x02);

    REQUIRE(adapter.hostSpecialDown(Trs80M3Keyboard::SpecialKey::CapsLock));
    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x06);

    adapter.hostSpecialUp(Trs80M3Keyboard::SpecialKey::Control);
    adapter.hostSpecialUp(Trs80M3Keyboard::SpecialKey::CapsLock);
    for (uint8_t i = 0; i < Trs80M3HostKeyboardAdapter::kReleaseHoldFrames; ++i)
        adapter.tick();

    REQUIRE(keyboard.read(Trs80M3Keyboard::rowOffset(7)) == 0x00);
}

TEST_CASE("TRS-80 Model III host keyboard bridge attaches to machine", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    auto bridge = Trs80M3HostKeyboardBridge::fromMachine(*machine);
    REQUIRE(bridge.has_value());

    REQUIRE(bridge->adapter().hostKeyDown('H'));
    REQUIRE(machine->bus().read(Trs80M3Keyboard::rowAddress(1)) == 0x01);
    bridge->adapter().hostKeyUp('H');
    for (uint8_t i = 0; i < Trs80M3HostKeyboardAdapter::kReleaseHoldFrames; ++i)
        bridge->adapter().tick();
    REQUIRE(machine->bus().read(Trs80M3Keyboard::rowAddress(1)) == 0x00);
}
