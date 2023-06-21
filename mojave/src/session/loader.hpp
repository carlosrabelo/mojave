#ifndef MOJAVE_LOADER_HPP
#define MOJAVE_LOADER_HPP

#include <cstdint>
#include "bus/bus.hpp"
#include "devices/shared/memory.hpp"

namespace loader {

bool loadBinary(const char* path, Bus& bus, uint16_t address);
bool loadBinary(const char* path, Memory& mem, uint16_t address);

} // namespace loader

#endif
