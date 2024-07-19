#ifndef MOJAVE_ZX80_HOST_KEYBOARD_BRIDGE_HPP
#define MOJAVE_ZX80_HOST_KEYBOARD_BRIDGE_HPP

#include <optional>
#include "devices/sinclair/host_keyboard_adapter.hpp"

class QKeyEvent;
class Machine;
class SinclairKeyboard;

class Zx80HostKeyboardBridge {
public:
    static std::optional<Zx80HostKeyboardBridge> fromMachine(Machine& machine);

    void tick();
    bool handleSdlKeyboardEvent(const void* sdl_event, Machine* machine = nullptr);
    bool handleQtKeyEvent(QKeyEvent* event, Machine* machine = nullptr);

    SinclairHostKeyboardAdapter& adapter() { return adapter_; }

private:
    explicit Zx80HostKeyboardBridge(SinclairKeyboard& keyboard);

    SinclairHostKeyboardAdapter adapter_;
};

#endif
