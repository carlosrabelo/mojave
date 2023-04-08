#ifndef MOJAVE_BUS_HPP
#define MOJAVE_BUS_HPP

#include <cstdint>
#include <vector>
#include "devices/device.hpp"

struct Mapping {
    Device* device;
    uint16_t start;
    uint16_t end_exclusive;
};

class Bus {
public:
    void attach(Device& device, uint16_t start, uint16_t end_exclusive);
    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);

    const Mapping* find(uint16_t address) const;

private:
    std::vector<Mapping> mappings_;
};


#endif
