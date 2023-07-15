#ifndef MOJAVE_TRS80M1_PRINTER_STATUS_HPP
#define MOJAVE_TRS80M1_PRINTER_STATUS_HPP

#include <cstdint>
#include "devices/device.hpp"

class Trs80M1PrinterStatus : public Device {
public:
    static constexpr uint8_t kReadyBit = 0x01;
    static constexpr uint8_t kIdleReadValue = 0xFE;   // bit 0 clear = printer ready
    static constexpr uint8_t kNotReadyReadValue = 0xFF;

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    void setReady(bool ready) { ready_ = ready; }
    bool ready() const { return ready_; }

private:
    bool ready_ = true;
};

#endif
