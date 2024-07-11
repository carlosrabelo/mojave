#include "devices/zx80/port_decode.hpp"

Zx80PortDecode::Zx80PortDecode(SinclairKeyboard& keyboard) : keyboard_(keyboard) {}

uint8_t Zx80PortDecode::read(uint16_t /*address*/) {
    return 0xFF;
}

void Zx80PortDecode::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Zx80PortDecode::reset() {}

uint8_t Zx80PortDecode::readPort(uint16_t port) {
    if ((port & 0x0001u) != 0)
        return 0xFF;
    return keyboard_.readPort(port);
}

void Zx80PortDecode::writePort(uint16_t /*port*/, uint8_t /*value*/) {}
