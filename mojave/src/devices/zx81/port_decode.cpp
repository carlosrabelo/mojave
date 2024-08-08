#include "devices/zx81/port_decode.hpp"

Zx81PortDecode::Zx81PortDecode(SinclairKeyboard& keyboard) : keyboard_(keyboard) {}

uint8_t Zx81PortDecode::read(uint16_t /*address*/) {
    return 0xFF;
}

void Zx81PortDecode::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Zx81PortDecode::reset() {
    cassette_.reset();
    nmi_generator_on_ = false;
}

uint8_t Zx81PortDecode::readPort(uint16_t port) {
    if ((port & 0x0001u) != 0)
        return 0xFF;

    cassette_.onFeRead();

    const auto row = SinclairKeyboard::rowFromPort(port);
    const uint8_t keys = row.has_value() ? keyboard_.readPort(port) : 0xFF;
    return static_cast<uint8_t>((keys & 0x7Fu) | cassette_.earBits());
}

void Zx81PortDecode::writePort(uint16_t port, uint8_t /*value*/) {
    cassette_.onAnyOut();
    // A0=0 (port FE family): start NMI generator. A0=1 (FD family): stop it.
    nmi_generator_on_ = ((port & 0x0001u) == 0);
}
