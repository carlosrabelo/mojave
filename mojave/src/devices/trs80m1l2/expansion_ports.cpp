#include "devices/trs80m1l2/expansion_ports.hpp"

uint8_t Trs80M1L2ExpansionPorts::read(uint16_t address) {
    return readPort(address);
}

void Trs80M1L2ExpansionPorts::write(uint16_t address, uint8_t value) {
    writePort(address, value);
}

uint8_t Trs80M1L2ExpansionPorts::readPort(uint16_t port) {
    switch (port) {
    case kSerialOffset:
        return kSerialStatusMask;
    case kBaudOffset:
        return kDipSwitchReadValue;
    case kUartOffset:
        return static_cast<uint8_t>(kUartTxEmptyMask | (rx_full_ ? 0x80u : 0x00u));
    case kSerialDataOffset:
        if (!rx_full_)
            return 0x00;
        rx_full_ = false;
        return rx_byte_;
    case kPrinterOffset:
        return kPrinterPortStatus;
    default:
        return 0xFF;
    }
}

void Trs80M1L2ExpansionPorts::writePort(uint16_t port, uint8_t value) {
    switch (port) {
    case kSerialOffset:
        rx_full_ = false;
        uart_control_ = 0;
        break;
    case kBaudOffset:
        baud_latch_ = value;
        break;
    case kUartOffset:
        uart_control_ = value;
        break;
    case kSerialDataOffset:
        tx_byte_ = value;
        break;
    case kPrinterOffset:
        printer_byte_ = value;
        break;
    default:
        break;
    }
}

void Trs80M1L2ExpansionPorts::reset() {
    baud_latch_ = 0;
    uart_control_ = 0;
    tx_byte_ = 0;
    rx_byte_ = 0;
    rx_full_ = false;
    printer_byte_ = 0;
}

void Trs80M1L2ExpansionPorts::queueRxByte(uint8_t value) {
    rx_byte_ = value;
    rx_full_ = true;
}
