#include "memory.hpp"

Memory::Memory(uint16_t size, bool read_only)
    : data_(size, 0), read_only_(read_only) {}

uint8_t Memory::read(uint16_t address) {
    if (address >= data_.size())
        return 0;
    return data_[address];
}

void Memory::write(uint16_t address, uint8_t value) {
    if (read_only_)
        return;
    if (address >= data_.size())
        return;
    data_[address] = value;
}

void Memory::load(uint16_t offset, const uint8_t* data, uint16_t size) {
    auto end = static_cast<uint16_t>(offset + size);
    if (end > data_.size())
        end = data_.size();
    for (uint16_t i = offset; i < end; ++i)
        data_[i] = data[i - offset];
}

uint8_t* Memory::directPointer(uint16_t offset) {
    if (offset < data_.size()) {
        return &data_[offset];
    }
    return nullptr;
}

