#include "devices/shared/virtual_tty.hpp"

// Port I/O: base = data, base+1 = status
static constexpr uint16_t TTY_DATA = 0;
static constexpr uint16_t TTY_STATUS = 1;

uint8_t VirtualTTY::readPort(uint16_t port) {
    switch (port & 1) {
    case TTY_DATA: {
        if (rx_count == 0) return 0;
        uint8_t c = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) % TTY_BUF_SIZE;
        rx_count--;
        return c;
    }
    case TTY_STATUS:
        return (rxHasData() ? 0x01 : 0) | (txHasSpace() ? 0x02 : 0);
    }
    return 0;
}

void VirtualTTY::writePort(uint16_t port, uint8_t value) {
    if ((port & 1) != TTY_DATA) return;
    if (tx_count >= TTY_BUF_SIZE) return;
    tx_buf[tx_head] = value;
    tx_head = (tx_head + 1) % TTY_BUF_SIZE;
    tx_count++;
}

// Memory-mapped: same layout at consecutive addresses
uint8_t VirtualTTY::read(uint16_t address) {
    return readPort(address & 1);
}

void VirtualTTY::write(uint16_t address, uint8_t value) {
    writePort(address & 1, value);
}

void VirtualTTY::reset() {
    rx_head = rx_tail = rx_count = 0;
    tx_head = tx_tail = tx_count = 0;
}

void VirtualTTY::injectChar(uint8_t c) {
    if (rx_count >= TTY_BUF_SIZE) return;
    rx_buf[rx_head] = c;
    rx_head = (rx_head + 1) % TTY_BUF_SIZE;
    rx_count++;
}

int VirtualTTY::readChar() {
    if (tx_count == 0) return -1;
    uint8_t c = tx_buf[tx_tail];
    tx_tail = (tx_tail + 1) % TTY_BUF_SIZE;
    tx_count--;
    return c;
}
