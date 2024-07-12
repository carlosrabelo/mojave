#include "catch.hpp"
#include "devices/sinclair/cassette.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/zx80/port_decode.hpp"

TEST_CASE("ZX-80 port decode merges keyboard and EAR on FE reads", "[device][zx80][fast]") {
    SinclairKeyboard keyboard;
    Zx80PortDecode ports(keyboard);

    REQUIRE(ports.readPort(0xFEFE) == 0xFF);

    keyboard.pressKey(SinclairKeyboard::Key::Shift);
    REQUIRE(ports.readPort(0xFEFE) == 0xFE);

    ports.cassette().setEarHigh(false);
    REQUIRE(ports.readPort(0xFEFE) == 0x7E);
}

TEST_CASE("ZX-80 port decode MIC drops on FE read and rises on any OUT", "[device][zx80][fast]") {
    SinclairKeyboard keyboard;
    Zx80PortDecode ports(keyboard);

    ports.cassette().onAnyOut();
    REQUIRE(ports.cassette().micHigh());

    ports.readPort(0x7FFE);
    REQUIRE_FALSE(ports.cassette().micHigh());

    ports.writePort(0x00FF, 0x00);
    REQUIRE(ports.cassette().micHigh());
}

TEST_CASE("ZX-80 port decode ignores odd ports for keyboard demux", "[device][zx80][fast]") {
    SinclairKeyboard keyboard;
    Zx80PortDecode ports(keyboard);
    keyboard.pressKey(SinclairKeyboard::Key::A);
    REQUIRE(ports.readPort(0xFDFE) == 0xFE);
    REQUIRE(ports.readPort(0xFDFF) == 0xFF);
}
