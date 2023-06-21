#ifndef MOJAVE_VIRTUAL_TTY_HPP
#define MOJAVE_VIRTUAL_TTY_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

constexpr unsigned TTY_BUF_SIZE = 256;

class VirtualTTY : public Device, public PortDevice {
public:
    VirtualTTY() = default;

    // Port I/O interface (Z80 style)
    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;

    // Memory-mapped interface (6502 style)
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;

    void reset() override;

    // Host API: inject character into RX (as if typed on keyboard)
    void injectChar(uint8_t c);
    // Host API: read character from TX (as if sent to terminal)
    int readChar();

    // Status queries
    bool rxHasData() const { return rx_count > 0; }
    bool txHasSpace() const { return tx_count < TTY_BUF_SIZE; }

private:
    uint8_t rx_buf[TTY_BUF_SIZE]{};
    unsigned rx_head = 0, rx_tail = 0, rx_count = 0;

    uint8_t tx_buf[TTY_BUF_SIZE]{};
    unsigned tx_head = 0, tx_tail = 0, tx_count = 0;
};

#endif
