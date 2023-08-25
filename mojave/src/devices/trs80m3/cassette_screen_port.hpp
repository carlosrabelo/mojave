#ifndef MOJAVE_TRS80M3_CASSETTE_SCREEN_PORT_HPP
#define MOJAVE_TRS80M3_CASSETTE_SCREEN_PORT_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

class Trs80M3VideoController;

class Trs80M3CassetteScreenPort : public Device, public PortDevice {
public:
    static constexpr uint16_t kPortStart = 0xFC;
    static constexpr uint16_t kPortEndExclusive = 0x100;
    static constexpr uint8_t kCassetteOutMask = 0x03;
    static constexpr uint8_t kMotorMask = 0x04;
    static constexpr uint8_t kWideScreenMask = 0x08;
    static constexpr uint8_t kCassetteInShift = 7;
    static constexpr uint8_t kCassetteInMask = 0x01;
    static constexpr uint8_t kIdleCassetteRead = 0x80;

    explicit Trs80M3CassetteScreenPort(Trs80M3VideoController& video);

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t cassetteOut() const { return cassette_out_; }
    bool motorOn() const { return motor_on_; }
    bool wideScreen() const { return wide_screen_; }
    uint8_t cassetteIn() const { return cassette_in_; }

    void setCassetteIn(uint8_t value);

private:
    Trs80M3VideoController& video_;
    uint8_t cassette_out_ = 0;
    bool motor_on_ = false;
    bool wide_screen_ = false;
    uint8_t cassette_in_ = kCassetteInMask;
};

#endif
