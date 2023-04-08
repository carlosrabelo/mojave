#include "bus.hpp"
#include <stdexcept>

void Bus::attach(Device& device, uint16_t start, uint16_t end_exclusive) {
    uint32_t new_end = end_exclusive == 0 ? 65536 : end_exclusive;
    for (const auto& m : mappings_) {
        uint32_t m_end = m.end_exclusive == 0 ? 65536 : m.end_exclusive;
        if (start < m_end && new_end > m.start)
            throw std::runtime_error("Bus: overlapping attach");
    }
    mappings_.push_back({&device, start, end_exclusive});
}

const Mapping* Bus::find(uint16_t address) const {
    for (const auto& m : mappings_) {
        if (address >= m.start && (m.end_exclusive == 0 || address < m.end_exclusive))
            return &m;
    }
    return nullptr;
}

uint8_t Bus::read(uint16_t address) const {
    auto* m = find(address);
    return m ? m->device->read(address - m->start) : 0xFF;
}

void Bus::write(uint16_t address, uint8_t value) {
    auto* m = find(address);
    if (m)
        m->device->write(address - m->start, value);
}
