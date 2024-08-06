#ifndef MOJAVE_ZX81_PORT_DECODE_HPP
#define MOJAVE_ZX81_PORT_DECODE_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"
#include "devices/sinclair/cassette.hpp"
#include "devices/sinclair/keyboard.hpp"

// ZX-81 I/O port demux: keyboard matrix reads on 0xFE-family ports plus
// cassette EAR/MIC side effects. NMI generator is wired in a later item.
class Zx81PortDecode : public Device, public PortDevice {
public:
    explicit Zx81PortDecode(SinclairKeyboard& keyboard);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;

    const SinclairCassette& cassette() const { return cassette_; }
    SinclairCassette& cassette() { return cassette_; }

private:
    SinclairKeyboard& keyboard_;
    SinclairCassette cassette_;
};

#endif
