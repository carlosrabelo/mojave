#ifndef MOJAVE_TRS80M3_HOST_KEYBOARD_BRIDGE_HPP
#define MOJAVE_TRS80M3_HOST_KEYBOARD_BRIDGE_HPP

#include <optional>
#include "machines/trs80m3/host_keyboard_adapter.hpp"

class QKeyEvent;
class Machine;
class Trs80M3Keyboard;

class Trs80M3HostKeyboardBridge {
public:
    static std::optional<Trs80M3HostKeyboardBridge> fromMachine(Machine& machine);

    void tick();
    bool handleSdlKeyboardEvent(const void* sdl_event, Machine* machine = nullptr);
    bool handleQtKeyEvent(QKeyEvent* event, Machine* machine = nullptr);

    Trs80M3HostKeyboardAdapter& adapter() { return adapter_; }

private:
    explicit Trs80M3HostKeyboardBridge(Trs80M3Keyboard& keyboard);

    Trs80M3HostKeyboardAdapter adapter_;
};

#endif
