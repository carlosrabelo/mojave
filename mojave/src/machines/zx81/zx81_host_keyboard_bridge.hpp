#ifndef MOJAVE_ZX81_HOST_KEYBOARD_BRIDGE_HPP
#define MOJAVE_ZX81_HOST_KEYBOARD_BRIDGE_HPP

#include <optional>
#include "devices/sinclair/host_keyboard_adapter.hpp"

class QKeyEvent;
class Machine;
class SinclairKeyboard;

class Zx81HostKeyboardBridge {
public:
    static std::optional<Zx81HostKeyboardBridge> fromMachine(Machine& machine);

    void tick();
    bool handleSdlKeyboardEvent(const void* sdl_event, Machine* machine = nullptr);
    bool handleQtKeyEvent(QKeyEvent* event, Machine* machine = nullptr);

    SinclairHostKeyboardAdapter& adapter() { return adapter_; }

private:
    explicit Zx81HostKeyboardBridge(SinclairKeyboard& keyboard);

    SinclairHostKeyboardAdapter adapter_;
};

#endif
