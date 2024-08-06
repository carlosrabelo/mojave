#include <cstdint>
#include "catch.hpp"
#include "devices/sinclair/cassette.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/zx81/port_decode.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("ZX-81 cassette contract matches Sinclair stub", "[device][zx81][fast]") {
    REQUIRE(SinclairCassette::kBaud == Zx81PresetContract::cassette_baud);
    REQUIRE(SinclairCassette::kEarBit == Zx81PresetContract::cassette_ear_bit);
}

TEST_CASE("ZX-81 cassette stub exposes idle EAR high on port reads", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);

    REQUIRE((ports.readPort(0x7FFE) & SinclairCassette::kEarBit) != 0);

    ports.cassette().setEarHigh(false);
    REQUIRE((ports.readPort(0x7FFE) & SinclairCassette::kEarBit) == 0);
}

TEST_CASE("ZX-81 cassette stub toggles MIC on IN FE and OUT", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);

    ports.cassette().onAnyOut();
    REQUIRE(ports.cassette().micHigh());

    ports.readPort(0x7FFE);
    REQUIRE_FALSE(ports.cassette().micHigh());

    ports.writePort(0x00FF, 0x09);
    REQUIRE(ports.cassette().micHigh());
}

TEST_CASE("ZX-81 port decode keeps keyboard reads through cassette merge", "[device][zx81][fast]") {
    SinclairKeyboard keyboard;
    Zx81PortDecode ports(keyboard);

    keyboard.pressKey(SinclairKeyboard::Key::Q);
    REQUIRE(ports.readPort(0xFBFE) == 0xFE);
    keyboard.releaseKey(SinclairKeyboard::Key::Q);
    REQUIRE(ports.readPort(0xFBFE) == 0xFF);
}

TEST_CASE("ZX-81 machine routes cassette and keyboard through port decode", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    SinclairKeyboard* keyboard = nullptr;
    Zx81PortDecode* ports = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* kb = dynamic_cast<SinclairKeyboard*>(dev.get()))
            keyboard = kb;
        if (auto* io = dynamic_cast<Zx81PortDecode*>(dev.get()))
            ports = io;
    }
    REQUIRE(keyboard != nullptr);
    REQUIRE(ports != nullptr);

    keyboard->pressKey(SinclairKeyboard::Key::Space);
    REQUIRE(machine->bus().readPort(0x7FFE) == 0xFE);
    keyboard->releaseKey(SinclairKeyboard::Key::Space);

    machine->bus().writePort(0x00FF, 0x04);
    REQUIRE(ports->cassette().micHigh());
}
