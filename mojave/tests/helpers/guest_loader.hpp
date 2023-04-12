#ifndef MOJAVE_GUEST_LOADER_HPP
#define MOJAVE_GUEST_LOADER_HPP

#include <cstdint>
#include <cstdio>
#include <vector>
#include "devices/shared/memory.hpp"

inline bool loadGuest(const char* path, Memory& mem, uint16_t offset) {
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;

    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    std::vector<uint8_t> buf(size);
    if (std::fread(buf.data(), 1, size, f) != static_cast<size_t>(size)) {
        std::fclose(f);
        return false;
    }
    std::fclose(f);

    mem.load(offset, buf.data(), static_cast<uint16_t>(size));
    return true;
}

#endif
