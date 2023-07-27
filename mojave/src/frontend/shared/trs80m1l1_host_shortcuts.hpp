#ifndef MOJAVE_TRS80M1L1_HOST_SHORTCUTS_HPP
#define MOJAVE_TRS80M1L1_HOST_SHORTCUTS_HPP

#include "devices/trs80m1/keyboard.hpp"

inline bool trs80m1l1HostCtrlShortcut(char key, Trs80M1Keyboard::SpecialKey& out) {
    if (key >= 'a' && key <= 'z')
        key = static_cast<char>(key - 'a' + 'A');

    switch (key) {
    case 'C':
        out = Trs80M1Keyboard::SpecialKey::Break;
        return true;
    case 'L':
        out = Trs80M1Keyboard::SpecialKey::Clear;
        return true;
    default:
        return false;
    }
}

#endif
