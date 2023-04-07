#ifndef MOJAVE_MEMORY_HPP
#define MOJAVE_MEMORY_HPP

#include <cstdint>
#include <vector>
#include "devices/device.hpp"

class Memory : public Device {
public:
    Memory(uint16_t size, bool read_only = false);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;

    void load(uint16_t offset, const uint8_t* data, uint16_t size);
    bool isReadOnly() const override { return read_only_; }
    uint8_t* directPointer(uint16_t offset) override;


private:
    std::vector<uint8_t> data_;
    bool read_only_;
};

#endif
