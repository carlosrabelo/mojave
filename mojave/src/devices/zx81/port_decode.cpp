#include "devices/zx81/port_decode.hpp"

Zx81PortDecode::Zx81PortDecode(SinclairKeyboard& keyboard) : keyboard_(keyboard) {}

uint8_t Zx81PortDecode::read(uint16_t /*address*/) {
    return 0xFF;
}

void Zx81PortDecode::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Zx81PortDecode::reset() {}

uint8_t Zx81PortDecode::readPort(uint16_t port) {
    if ((port & 0x0001u) != 0)
        return 0xFF;

    const auto row = SinclairKeyboard::rowFromPort(port);
    return row.has_value() ? keyboard_.readPort(port) : 0xFF;
}

void Zx81PortDecode::writePort(uint16_t /*port*/, uint8_t /*value*/) {}
