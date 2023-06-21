#include "session/loader.hpp"
#include "devices/shared/memory.hpp"
#include <algorithm>
#include <cstdio>
#include <vector>

namespace loader {

bool loadBinary(const char* path, Memory& mem, uint16_t address) {
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;

    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    if (size <= 0) {
        std::fclose(f);
        return false;
    }
    std::fseek(f, 0, SEEK_SET);

    std::vector<uint8_t> buf(size);
    if (std::fread(buf.data(), 1, size, f) != static_cast<size_t>(size)) {
        std::fclose(f);
        return false;
    }
    std::fclose(f);

    mem.load(address, buf.data(), static_cast<uint16_t>(size));
    return true;
}

bool loadBinary(const char* path, Bus& bus, uint16_t address) {
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;

    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    if (size <= 0) {
        std::fclose(f);
        return false;
    }
    std::fseek(f, 0, SEEK_SET);

    std::vector<uint8_t> buf(size);
    if (std::fread(buf.data(), 1, size, f) != static_cast<size_t>(size)) {
        std::fclose(f);
        return false;
    }
    std::fclose(f);

    uint16_t offset = 0;
    while (offset < static_cast<uint16_t>(size)) {
        const uint16_t addr = static_cast<uint16_t>(address + offset);
        const Mapping* mapping = bus.find(addr);
        if (!mapping)
            return false;

        const uint32_t map_end =
            mapping->end_exclusive == 0 ? 65536u : static_cast<uint32_t>(mapping->end_exclusive);
        const uint16_t device_offset = static_cast<uint16_t>(addr - mapping->start);
        const uint16_t remaining_in_map = static_cast<uint16_t>(map_end - addr);
        const uint16_t chunk =
            static_cast<uint16_t>(std::min<uint32_t>(remaining_in_map, size - offset));

        if (auto* mem = dynamic_cast<Memory*>(mapping->device)) {
            mem->load(device_offset, buf.data() + offset, chunk);
        } else {
            for (uint16_t i = 0; i < chunk; ++i)
                mapping->device->write(static_cast<uint16_t>(device_offset + i), buf[offset + i]);
        }

        offset = static_cast<uint16_t>(offset + chunk);
    }
    return true;
}

} // namespace loader
