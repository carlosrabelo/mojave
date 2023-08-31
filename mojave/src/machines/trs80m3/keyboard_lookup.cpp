#include "machines/trs80m3/keyboard_lookup.hpp"
#include "devices/trs80m3/keyboard.hpp"
#include "machines/shared/machine.hpp"

Trs80M3Keyboard* findTrs80M3Keyboard(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* keyboard = dynamic_cast<Trs80M3Keyboard*>(dev.get()))
            return keyboard;
    }
    return nullptr;
}
