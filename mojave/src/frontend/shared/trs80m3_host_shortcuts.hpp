#ifndef MOJAVE_TRS80M3_HOST_SHORTCUTS_HPP
#define MOJAVE_TRS80M3_HOST_SHORTCUTS_HPP

#include "devices/trs80m3/keyboard.hpp"

inline bool trs80m3HostCtrlShortcut(char key, Trs80M3Keyboard::SpecialKey& out) {
    if (key >= 'a' && key <= 'z')
        key = static_cast<char>(key - 'a' + 'A');

    switch (key) {
    case 'C':
        out = Trs80M3Keyboard::SpecialKey::Break;
        return true;
    case 'L':
        out = Trs80M3Keyboard::SpecialKey::Clear;
        return true;
    default:
        return false;
    }
}

#endif
