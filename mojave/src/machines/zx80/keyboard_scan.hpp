#ifndef MOJAVE_ZX80_KEYBOARD_SCAN_HPP
#define MOJAVE_ZX80_KEYBOARD_SCAN_HPP

#include <cstdint>
#include "devices/sinclair/keyboard.hpp"

// Simulates the ZX-80 ROM keyboard scan at 0x0143 (HL = row/column bitmask).
uint16_t zx80SimulateRomKeyboardScan(SinclairKeyboard& keyboard);

#endif
