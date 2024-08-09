#include "machines/zx81/zx81_host_keyboard_bridge.hpp"
#include "devices/sinclair/keyboard_lookup.hpp"
#include "devices/zx81/port_decode.hpp"
#include "machines/shared/machine.hpp"

Zx81HostKeyboardBridge::Zx81HostKeyboardBridge(SinclairKeyboard& keyboard) : adapter_(keyboard) {}

std::optional<Zx81HostKeyboardBridge> Zx81HostKeyboardBridge::fromMachine(Machine& machine) {
    SinclairKeyboard* keyboard = findSinclairKeyboard(machine);
    if (!keyboard)
        return std::nullopt;

    // ZX-80 also owns a SinclairKeyboard; require the ZX-81 port decode.
    bool has_zx81_ports = false;
    for (const auto& dev : machine.ownedDevices()) {
        if (dynamic_cast<Zx81PortDecode*>(dev.get())) {
            has_zx81_ports = true;
            break;
        }
    }
    if (!has_zx81_ports)
        return std::nullopt;

    return Zx81HostKeyboardBridge(*keyboard);
}

void Zx81HostKeyboardBridge::tick() {
    adapter_.tick();
}

#ifndef MOJAVE_FRONTEND_SDL
bool Zx81HostKeyboardBridge::handleSdlKeyboardEvent(const void* /*sdl_event*/, Machine* /*machine*/) {
    return false;
}
#endif

#ifndef MOJAVE_FRONTEND_QT6
bool Zx81HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* /*event*/, Machine* /*machine*/) {
    return false;
}
#endif
