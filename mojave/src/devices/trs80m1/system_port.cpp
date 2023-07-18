#include "devices/trs80m1/system_port.hpp"
#include "devices/trs80m1/video_controller.hpp"

Trs80M1SystemPort::Trs80M1SystemPort(Trs80M1VideoController& video) : video_(video) {}

uint8_t Trs80M1SystemPort::read(uint16_t address) {
    return readPort(address);
}

void Trs80M1SystemPort::write(uint16_t address, uint8_t value) {
    writePort(address, value);
}

uint8_t Trs80M1SystemPort::readPort(uint16_t /*port*/) {
    return static_cast<uint8_t>((cassette_in_ & kCassetteInMask) << kCassetteInShift);
}

void Trs80M1SystemPort::writePort(uint16_t /*port*/, uint8_t value) {
    cassette_out_ = static_cast<uint8_t>(value & kCassetteOutMask);
    motor_on_ = (value & kMotorMask) != 0;

    const bool wide = (value & kWideScreenMask) != 0;
    if (wide != wide_screen_) {
        wide_screen_ = wide;
        video_.setWideMode(wide);
    }
}

void Trs80M1SystemPort::reset() {
    cassette_out_ = 0;
    motor_on_ = false;
    cassette_in_ = kCassetteInMask;
    if (wide_screen_) {
        wide_screen_ = false;
        video_.setWideMode(false);
    }
}

void Trs80M1SystemPort::setCassetteIn(uint8_t value) {
    cassette_in_ = static_cast<uint8_t>(value & kCassetteInMask);
}
