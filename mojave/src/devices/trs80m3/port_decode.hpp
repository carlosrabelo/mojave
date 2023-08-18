#ifndef MOJAVE_TRS80M3_PORT_DECODE_HPP
#define MOJAVE_TRS80M3_PORT_DECODE_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

class Z80;

class Trs80M3PortDecode : public Device, public PortDevice {
public:
    static constexpr uint16_t kPortStart = 0xE0;
    static constexpr uint16_t kPortEndExclusive = 0xF0;

    static constexpr uint8_t kInterruptOffset = 0x00;      // E0–E3
    static constexpr uint8_t kNmiOffset = 0x04;            // E4–E7
    static constexpr uint8_t kRs232StatusOffset = 0x08;    // E8–EB
    static constexpr uint8_t kHardwareControlOffset = 0x0C; // EC–EF

    static constexpr uint8_t kRs232ModemOffset = 0x00;     // E8 within RS-232 block
    static constexpr uint8_t kRs232BaudOffset = 0x01;      // E9
    static constexpr uint8_t kRs232UartOffset = 0x02;      // EA
    static constexpr uint8_t kRs232DataOffset = 0x03;      // EB

    static constexpr uint8_t kRtcInterruptMask = 0x04;
    static constexpr uint8_t kVideoWaitMask = 0x20;
    static constexpr uint8_t kIoBusMask = 0x10;
    static constexpr uint8_t kAltCharMask = 0x08;
    static constexpr uint8_t kDoubleWidthMask = 0x04;
    static constexpr uint8_t kCassetteMotorOffMask = 0x02;

    static constexpr uint8_t kRs232IdleStatus = 0xF0;
    static constexpr uint8_t kUartTxEmptyMask = 0x40;
    static constexpr uint8_t kDipSwitchReadValue = 0x55;
    static constexpr uint8_t kNmiIdleStatus = 0x00;
    static constexpr uint8_t kDiskIndexStatus = 0x20;

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t interruptMask() const { return interrupt_mask_; }
    uint8_t interruptStatus() const { return interrupt_status_; }
    uint8_t nmiMask() const { return nmi_mask_; }
    uint8_t hardwareControl() const { return hardware_control_; }

    bool rtcEnabled() const { return (interrupt_mask_ & kRtcInterruptMask) != 0; }
    bool doubleWidth() const { return (hardware_control_ & kDoubleWidthMask) != 0; }
    bool cassetteMotorOn() const { return (hardware_control_ & kCassetteMotorOffMask) == 0; }
    bool videoWaitsEnabled() const { return (hardware_control_ & kVideoWaitMask) != 0; }

    uint8_t baudLatch() const { return baud_latch_; }
    uint8_t uartControl() const { return uart_control_; }
    uint8_t lastTxByte() const { return tx_byte_; }
    bool rxFull() const { return rx_full_; }

    void queueRxByte(uint8_t value);

    void setDiskIndexPending(bool pending) { disk_index_pending_ = pending; }

    void setInterruptStatus(uint8_t status) { interrupt_status_ = status; }
    void setRtcPending(bool pending);

    void bindCpu(Z80* cpu) { cpu_ = cpu; }
    bool maskableInterruptActive() const;
    void syncMaskableInterrupt();

private:
    uint8_t blockOffset(uint16_t port) const { return static_cast<uint8_t>(port & 0x0C); }
    uint8_t rs232Offset(uint16_t port) const { return static_cast<uint8_t>(port & 0x03); }

    uint8_t interrupt_mask_ = 0;
    uint8_t interrupt_status_ = 0xFF; // active-low: 1 = idle, 0 = pending
    uint8_t nmi_mask_ = 0;
    uint8_t hardware_control_ = kCassetteMotorOffMask;
    uint8_t baud_latch_ = 0;
    uint8_t uart_control_ = 0;
    uint8_t tx_byte_ = 0;
    uint8_t rx_byte_ = 0;
    bool rx_full_ = false;
    bool disk_index_pending_ = false;
    Z80* cpu_ = nullptr;
};

#endif
