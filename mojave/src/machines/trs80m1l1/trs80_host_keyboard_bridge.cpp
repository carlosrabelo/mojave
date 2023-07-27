#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"
#include "machines/trs80m1l1/keyboard_lookup.hpp"

Trs80HostKeyboardBridge::Trs80HostKeyboardBridge(Trs80M1Keyboard& keyboard)
    : adapter_(keyboard) {}

std::optional<Trs80HostKeyboardBridge> Trs80HostKeyboardBridge::fromMachine(Machine& machine) {
    Trs80M1Keyboard* keyboard = findTrs80M1L1Keyboard(machine);
    if (!keyboard)
        return std::nullopt;
    return Trs80HostKeyboardBridge(*keyboard);
}

void Trs80HostKeyboardBridge::tick() {
    adapter_.tick();
}

#ifndef MOJAVE_FRONTEND_SDL
bool Trs80HostKeyboardBridge::handleSdlKeyboardEvent(const void* /*sdl_event*/, Machine* /*machine*/) {
    return false;
}
#endif

#ifndef MOJAVE_FRONTEND_QT6
bool Trs80HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* /*event*/, Machine* /*machine*/) {
    return false;
}
#endif
