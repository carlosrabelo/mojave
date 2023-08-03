#ifndef MOJAVE_TRS80M1L2_EXPANSION_PORTS_HPP
#define MOJAVE_TRS80M1L2_EXPANSION_PORTS_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

class Trs80M1L2ExpansionPorts : public Device, public PortDevice {
public:
    static constexpr uint16_t kPortStart = 0xE8;
    static constexpr uint16_t kPortEndExclusive = 0xF0;

    static constexpr uint8_t kSerialOffset = 0;
    static constexpr uint8_t kBaudOffset = 1;
    static constexpr uint8_t kUartOffset = 2;
    static constexpr uint8_t kSerialDataOffset = 3;
    static constexpr uint8_t kPrinterOffset = 4;

    static constexpr uint8_t kSerialStatusMask = 0xF0; // CTS, DSR, CD, RI high
    static constexpr uint8_t kUartTxEmptyMask = 0x40;
    static constexpr uint8_t kDipSwitchReadValue = 0x55; // 300 baud default
    static constexpr uint8_t kPrinterPortStatus = 0xF0;  // not busy, paper ok, selected, no fault

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t baudLatch() const { return baud_latch_; }
    uint8_t uartControl() const { return uart_control_; }
    uint8_t lastTxByte() const { return tx_byte_; }
    uint8_t lastPrinterByte() const { return printer_byte_; }
    bool rxFull() const { return rx_full_; }

    void queueRxByte(uint8_t value);

private:
    uint8_t baud_latch_ = 0;
    uint8_t uart_control_ = 0;
    uint8_t tx_byte_ = 0;
    uint8_t rx_byte_ = 0;
    bool rx_full_ = false;
    uint8_t printer_byte_ = 0;
};

#endif
