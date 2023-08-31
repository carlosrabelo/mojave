#include "machines/trs80m3/trs80_host_keyboard_bridge.hpp"
#include "machines/trs80m3/keyboard_lookup.hpp"

Trs80M3HostKeyboardBridge::Trs80M3HostKeyboardBridge(Trs80M3Keyboard& keyboard)
    : adapter_(keyboard) {}

std::optional<Trs80M3HostKeyboardBridge> Trs80M3HostKeyboardBridge::fromMachine(Machine& machine) {
    Trs80M3Keyboard* keyboard = findTrs80M3Keyboard(machine);
    if (!keyboard)
        return std::nullopt;
    return Trs80M3HostKeyboardBridge(*keyboard);
}

void Trs80M3HostKeyboardBridge::tick() {
    adapter_.tick();
}

#ifndef MOJAVE_FRONTEND_SDL
bool Trs80M3HostKeyboardBridge::handleSdlKeyboardEvent(const void* /*sdl_event*/, Machine* /*machine*/) {
    return false;
}
#endif

#ifndef MOJAVE_FRONTEND_QT6
bool Trs80M3HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* /*event*/, Machine* /*machine*/) {
    return false;
}
#endif
