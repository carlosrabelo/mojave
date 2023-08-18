#include "devices/trs80m3/port_decode.hpp"
#include "cpus/z80.hpp"

uint8_t Trs80M3PortDecode::read(uint16_t address) {
    return readPort(address);
}

void Trs80M3PortDecode::write(uint16_t address, uint8_t value) {
    writePort(address, value);
}

void Trs80M3PortDecode::setRtcPending(bool pending) {
    // Interrupt latch bits are active-low: 1 = idle, 0 = pending. Model III
    // ROM does IN A,(E0) / RRA / JP NC to dispatch, so a pending RTC must
    // clear bit 2 — setting it (active-high) sends the CPU into the cassette
    // service path and never reaches Cass?/READY.
    if (pending)
        interrupt_status_ = static_cast<uint8_t>(interrupt_status_ & ~kRtcInterruptMask);
    else
        interrupt_status_ = static_cast<uint8_t>(interrupt_status_ | kRtcInterruptMask);
    syncMaskableInterrupt();
}

bool Trs80M3PortDecode::maskableInterruptActive() const {
    return ((static_cast<uint8_t>(~interrupt_status_) & interrupt_mask_) != 0);
}

void Trs80M3PortDecode::syncMaskableInterrupt() {
    if (cpu_ != nullptr)
        cpu_->setIntLine(maskableInterruptActive());
}

void Trs80M3PortDecode::queueRxByte(uint8_t value) {
    rx_byte_ = value;
    rx_full_ = true;
}

uint8_t Trs80M3PortDecode::readPort(uint16_t port) {
    switch (blockOffset(port)) {
    case kInterruptOffset:
        return interrupt_status_;
    case kNmiOffset:
        if (disk_index_pending_) {
            disk_index_pending_ = false;
            return kDiskIndexStatus;
        }
        return kNmiIdleStatus;
    case kRs232StatusOffset:
        switch (rs232Offset(port)) {
        case kRs232ModemOffset:
            return kRs232IdleStatus;
        case kRs232BaudOffset:
            return kDipSwitchReadValue;
        case kRs232UartOffset:
            return static_cast<uint8_t>(kUartTxEmptyMask | (rx_full_ ? 0x80u : 0x00u));
        case kRs232DataOffset:
            if (!rx_full_)
                return 0x00;
            rx_full_ = false;
            return rx_byte_;
        default:
            return 0xFF;
        }
    case kHardwareControlOffset:
        // Reading EC–EF acknowledges / clears the RTC interrupt (bit back to 1).
        interrupt_status_ = static_cast<uint8_t>(interrupt_status_ | kRtcInterruptMask);
        syncMaskableInterrupt();
        return 0xFF;
    default:
        return 0xFF;
    }
}

void Trs80M3PortDecode::writePort(uint16_t port, uint8_t value) {
    switch (blockOffset(port)) {
    case kInterruptOffset:
        interrupt_mask_ = value;
        syncMaskableInterrupt();
        break;
    case kNmiOffset:
        nmi_mask_ = value;
        break;
    case kRs232StatusOffset:
        switch (rs232Offset(port)) {
        case kRs232ModemOffset:
            rx_full_ = false;
            uart_control_ = 0;
            break;
        case kRs232BaudOffset:
            baud_latch_ = value;
            break;
        case kRs232UartOffset:
            uart_control_ = value;
            break;
        case kRs232DataOffset:
            tx_byte_ = value;
            break;
        default:
            break;
        }
        break;
    case kHardwareControlOffset:
        hardware_control_ = value;
        break;
    default:
        break;
    }
}

void Trs80M3PortDecode::reset() {
    interrupt_mask_ = 0;
    interrupt_status_ = 0xFF; // all interrupt sources idle (active-low)
    nmi_mask_ = 0;
    hardware_control_ = kCassetteMotorOffMask;
    baud_latch_ = 0;
    uart_control_ = 0;
    tx_byte_ = 0;
    rx_byte_ = 0;
    rx_full_ = false;
    disk_index_pending_ = false;
    syncMaskableInterrupt();
}
