#ifndef MOJAVE_TRS80_HOST_KEYBOARD_BRIDGE_HPP
#define MOJAVE_TRS80_HOST_KEYBOARD_BRIDGE_HPP

#include <optional>
#include "machines/trs80m1l1/host_keyboard_adapter.hpp"

class QKeyEvent;
class Machine;
class Trs80M1Keyboard;

class Trs80HostKeyboardBridge {
public:
    static std::optional<Trs80HostKeyboardBridge> fromMachine(Machine& machine);

    void tick();
    bool handleSdlKeyboardEvent(const void* sdl_event, Machine* machine = nullptr);
    bool handleQtKeyEvent(QKeyEvent* event, Machine* machine = nullptr);

    Trs80M1L1HostKeyboardAdapter& adapter() { return adapter_; }

private:
    explicit Trs80HostKeyboardBridge(Trs80M1Keyboard& keyboard);

    Trs80M1L1HostKeyboardAdapter adapter_;
};

#endif
