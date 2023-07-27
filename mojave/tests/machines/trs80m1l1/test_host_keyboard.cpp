#include "catch.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/trs80m1l1/host_keyboard_adapter.hpp"
#include "machines/trs80m1l1/trs80m1l1_preset.hpp"
#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"
#include "machines/trs80m1l1/typing_chord.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("TRS-80 Model I Level I host keyboard adapter holds release for several frames",
          "[machine][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);

    adapter.hostKeyDown('A');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);

    adapter.hostKeyUp('A');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);

    for (uint8_t i = 1; i < Trs80M1L1HostKeyboardAdapter::kReleaseHoldFrames; ++i) {
        adapter.tick();
        REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x02);
    }

    adapter.tick();
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(0)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I host keyboard adapter pulses shifted typing chords",
          "[machine][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);

    adapter.pulseCharacter('"');
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(4)) == 0x04);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x01);

    for (uint8_t i = 0; i < Trs80M1L1HostKeyboardAdapter::kPulseFrames - 1; ++i)
        adapter.tick();

    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(4)) == 0x04);
    adapter.tick();
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(4)) == 0x00);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I host keyboard adapter keeps shift down until typing key releases",
          "[machine][trs80m1l1][fast]") {
    Trs80M1Keyboard keyboard;
    Trs80M1L1HostKeyboardAdapter adapter(keyboard);

    adapter.syncHostShift(true);
    adapter.hostKeyDown('2');
    adapter.syncHostShift(false);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x01);

    adapter.hostKeyUp('2');
    for (uint8_t i = 0; i < Trs80M1L1HostKeyboardAdapter::kReleaseHoldFrames; ++i)
        adapter.tick();

    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(4)) == 0x00);
    REQUIRE(keyboard.read(Trs80M1Keyboard::rowOffset(7)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I host keyboard bridge attaches to machine", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();
    auto bridge = Trs80HostKeyboardBridge::fromMachine(*machine);
    REQUIRE(bridge.has_value());

    REQUIRE(bridge->adapter().hostKeyDown('H'));
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(1)) == 0x01);
    bridge->adapter().hostKeyUp('H');
    for (uint8_t i = 0; i < Trs80M1L1HostKeyboardAdapter::kReleaseHoldFrames; ++i)
        bridge->adapter().tick();
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(1)) == 0x00);
}
