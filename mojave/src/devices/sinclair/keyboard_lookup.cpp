#include "devices/sinclair/keyboard_lookup.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "machines/shared/machine.hpp"

SinclairKeyboard* findSinclairKeyboard(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* keyboard = dynamic_cast<SinclairKeyboard*>(dev.get()))
            return keyboard;
    }
    return nullptr;
}
