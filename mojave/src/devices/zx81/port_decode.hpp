#ifndef MOJAVE_ZX81_PORT_DECODE_HPP
#define MOJAVE_ZX81_PORT_DECODE_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"
#include "devices/sinclair/keyboard.hpp"

// ZX-81 I/O port demux: keyboard matrix reads on 0xFE-family ports
// (A0 low, row from A8–A15). Cassette and NMI generator are wired later.
class Zx81PortDecode : public Device, public PortDevice {
public:
    explicit Zx81PortDecode(SinclairKeyboard& keyboard);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;

private:
    SinclairKeyboard& keyboard_;
};

#endif
