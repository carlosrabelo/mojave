#include "machines/zx80/zx80_host_keyboard_bridge.hpp"
#include "devices/sinclair/keyboard_lookup.hpp"

Zx80HostKeyboardBridge::Zx80HostKeyboardBridge(SinclairKeyboard& keyboard) : adapter_(keyboard) {}

std::optional<Zx80HostKeyboardBridge> Zx80HostKeyboardBridge::fromMachine(Machine& machine) {
    SinclairKeyboard* keyboard = findSinclairKeyboard(machine);
    if (!keyboard)
        return std::nullopt;
    return Zx80HostKeyboardBridge(*keyboard);
}

void Zx80HostKeyboardBridge::tick() {
    adapter_.tick();
}

#ifndef MOJAVE_FRONTEND_SDL
bool Zx80HostKeyboardBridge::handleSdlKeyboardEvent(const void* /*sdl_event*/, Machine* /*machine*/) {
    return false;
}
#endif

#ifndef MOJAVE_FRONTEND_QT6
bool Zx80HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* /*event*/, Machine* /*machine*/) {
    return false;
}
#endif
