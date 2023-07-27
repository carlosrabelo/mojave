#include "machines/trs80m1l1/keyboard_lookup.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/shared/machine.hpp"

Trs80M1Keyboard* findTrs80M1L1Keyboard(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* keyboard = dynamic_cast<Trs80M1Keyboard*>(dev.get()))
            return keyboard;
    }
    return nullptr;
}
