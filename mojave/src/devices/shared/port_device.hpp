#ifndef MOJAVE_PORT_DEVICE_HPP
#define MOJAVE_PORT_DEVICE_HPP

#include <cstdint>

class PortDevice {
public:
    virtual ~PortDevice() = default;

    virtual uint8_t readPort(uint16_t port) = 0;
    virtual void writePort(uint16_t port, uint8_t value) = 0;
};

#endif
