#ifndef MOJAVE_DEVICE_HPP
#define MOJAVE_DEVICE_HPP

#include <cstdint>

class Device {
public:
    virtual ~Device() = default;

    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t value) = 0;

    virtual void reset() {}
    virtual void tick(unsigned /*cycles_elapsed*/) {}

    virtual uint8_t* directPointer(uint16_t /*offset*/) { return nullptr; }
    virtual bool isReadOnly() const { return false; }

};

#endif
