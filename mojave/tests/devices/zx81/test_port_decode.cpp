#include "catch.hpp"
#include "devices/sinclair/cassette.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/zx81/port_decode.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("ZX-81 port decode reads keyboard on FE-family ports", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);

    REQUIRE(ports.readPort(0xFEFE) == 0xFF);

    keyboard.pressKey(SinclairKeyboard::Key::Shift);
    REQUIRE(ports.readPort(0xFEFE) == 0xFE);

    keyboard.pressKey(SinclairKeyboard::Key::A);
    REQUIRE(ports.readPort(0xFDFE) == 0xFE);
}

TEST_CASE("ZX-81 port decode merges keyboard and EAR on FE reads", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);

    keyboard.pressKey(SinclairKeyboard::Key::Shift);
    REQUIRE(ports.readPort(0xFEFE) == 0xFE);

    ports.cassette().setEarHigh(false);
    REQUIRE(ports.readPort(0xFEFE) == 0x7E);
}

TEST_CASE("ZX-81 port decode MIC drops on FE read and rises on any OUT", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);

    ports.cassette().onAnyOut();
    REQUIRE(ports.cassette().micHigh());

    ports.readPort(0x7FFE);
    REQUIRE_FALSE(ports.cassette().micHigh());

    ports.writePort(0x00FF, 0x00);
    REQUIRE(ports.cassette().micHigh());
}

TEST_CASE("ZX-81 port decode ignores odd ports for keyboard demux", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);
    keyboard.pressKey(SinclairKeyboard::Key::A);
    REQUIRE(ports.readPort(0xFDFE) == 0xFE);
    REQUIRE(ports.readPort(0xFDFF) == 0xFF);
}

TEST_CASE("ZX-81 machine IN reads the Sinclair keyboard matrix", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    SinclairKeyboard* keyboard = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* keys = dynamic_cast<SinclairKeyboard*>(dev.get()))
            keyboard = keys;
    }
    REQUIRE(keyboard != nullptr);

    REQUIRE(machine->bus().readPort(SinclairKeyboard::rowPort(1)) == 0xFF);
    keyboard->pressKey(SinclairKeyboard::Key::A);
    REQUIRE(machine->bus().readPort(SinclairKeyboard::rowPort(1)) == 0xFE);
}
