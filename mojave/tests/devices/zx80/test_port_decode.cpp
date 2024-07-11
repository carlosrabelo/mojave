#include "catch.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/zx80/port_decode.hpp"

TEST_CASE("ZX-80 port decode reads keyboard matrix on FE ports", "[device][zx80][fast]") {
    SinclairKeyboard keyboard;
    Zx80PortDecode ports(keyboard);

    REQUIRE(ports.readPort(0xFEFE) == 0xFF);

    keyboard.pressKey(SinclairKeyboard::Key::Shift);
    REQUIRE(ports.readPort(0xFEFE) == 0xFE);

    keyboard.pressKey(SinclairKeyboard::Key::A);
    REQUIRE(ports.readPort(0xFDFE) == 0xFE);
}

TEST_CASE("ZX-80 port decode ignores odd ports for keyboard demux", "[device][zx80][fast]") {
    SinclairKeyboard keyboard;
    Zx80PortDecode ports(keyboard);
    keyboard.pressKey(SinclairKeyboard::Key::A);
    REQUIRE(ports.readPort(0xFDFE) == 0xFE);
    REQUIRE(ports.readPort(0xFDFF) == 0xFF);
}
