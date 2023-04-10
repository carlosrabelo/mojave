#ifndef MOJAVE_BUS_HPP
#define MOJAVE_BUS_HPP

#include <cstdint>
#include <vector>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

struct Mapping {
    Device* device;
    uint16_t start;
    uint16_t end_exclusive;
};

struct PortMapping {
    PortDevice* device;
    uint16_t start;
    uint16_t end_exclusive;
    bool decode_low_byte = false;
};

class Bus {
public:
    void attach(Device& device, uint16_t start, uint16_t end_exclusive);
    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);

    void attachPort(PortDevice& device, uint16_t start, uint16_t end_exclusive,
                    bool decode_low_byte = false);
    uint8_t readPort(uint16_t port) const;
    void writePort(uint16_t port, uint8_t value);

    const Mapping* find(uint16_t address) const;

private:
    std::vector<Mapping> mappings_;
    std::vector<PortMapping> port_mappings_;
    const PortMapping* findPort(uint16_t port) const;
    static uint16_t portOffset(const PortMapping& mapping, uint16_t port);
};


#endif
