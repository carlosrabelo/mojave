#ifndef MOJAVE_ZX80_PORT_DECODE_HPP
#define MOJAVE_ZX80_PORT_DECODE_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"
#include "devices/sinclair/keyboard.hpp"

// ZX-80 I/O port demux: keyboard matrix reads on 0xFE-family ports.
// Cassette EAR/MIC side effects are added in a later TODO item.
class Zx80PortDecode : public Device, public PortDevice {
public:
    explicit Zx80PortDecode(SinclairKeyboard& keyboard);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;

private:
    SinclairKeyboard& keyboard_;
};

#endif
