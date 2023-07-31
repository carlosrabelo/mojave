#include "catch.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/trs80m1l1/host_keyboard_adapter.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("TRS-80 Model I Level II host keyboard bridge attaches to machine", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    auto bridge = Trs80HostKeyboardBridge::fromMachine(*machine);
    REQUIRE(bridge.has_value());

    REQUIRE(bridge->adapter().hostKeyDown('H'));
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(1)) == 0x01);
    bridge->adapter().hostKeyUp('H');
    for (uint8_t i = 0; i < Trs80M1L1HostKeyboardAdapter::kReleaseHoldFrames; ++i)
        bridge->adapter().tick();
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(1)) == 0x00);
}
