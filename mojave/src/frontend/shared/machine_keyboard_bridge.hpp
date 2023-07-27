#ifndef MOJAVE_MACHINE_KEYBOARD_BRIDGE_HPP
#define MOJAVE_MACHINE_KEYBOARD_BRIDGE_HPP

#include <optional>
#include <variant>
#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"

class Machine;
class QKeyEvent;

class MachineKeyboardBridge {
public:
    static std::optional<MachineKeyboardBridge> fromMachine(Machine& machine);

    void tick();
    bool handleSdlKeyboardEvent(const void* sdl_event, Machine* machine = nullptr);
    bool handleQtKeyEvent(QKeyEvent* event, Machine* machine = nullptr);

private:
    explicit MachineKeyboardBridge(Trs80HostKeyboardBridge bridge);

    std::variant<Trs80HostKeyboardBridge> bridge_;
};

#endif
