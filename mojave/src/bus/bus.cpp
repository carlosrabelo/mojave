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

void Bus::attachPort(PortDevice& device, uint16_t start, uint16_t end_exclusive,
                     bool decode_low_byte) {
    uint32_t new_end = end_exclusive == 0 ? 65536 : end_exclusive;
    for (const auto& m : port_mappings_) {
        uint32_t m_end = m.end_exclusive == 0 ? 65536 : m.end_exclusive;
        if (start < m_end && new_end > m.start)
            throw std::runtime_error("Bus: overlapping port attach");
    }
    port_mappings_.push_back({&device, start, end_exclusive, decode_low_byte});
}

const PortMapping* Bus::findPort(uint16_t port) const {
    for (const auto& m : port_mappings_) {
        uint32_t end = m.end_exclusive == 0 ? 65536 : m.end_exclusive;
        if (port >= m.start && port < end)
            return &m;
    }
    for (const auto& m : port_mappings_) {
        if (!m.decode_low_byte)
            continue;
        const uint16_t low = static_cast<uint16_t>(port & 0xFF);
        if (low >= m.start && low < m.end_exclusive)
            return &m;
    }
    return nullptr;
}

uint16_t Bus::portOffset(const PortMapping& mapping, uint16_t port) {
    uint32_t end = mapping.end_exclusive == 0 ? 65536 : mapping.end_exclusive;
    if (port >= mapping.start && port < end)
        return static_cast<uint16_t>(port - mapping.start);
    return static_cast<uint16_t>((port & 0xFF) - mapping.start);
}

uint8_t Bus::readPort(uint16_t port) const {
    auto* m = findPort(port);
    return m ? m->device->readPort(portOffset(*m, port)) : 0xFF;
}

void Bus::writePort(uint16_t port, uint8_t value) {
    auto* m = findPort(port);
    if (m)
        m->device->writePort(portOffset(*m, port), value);
}
