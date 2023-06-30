#ifndef MOJAVE_MACHINE_KEYBOARD_BRIDGE_HPP
#define MOJAVE_MACHINE_KEYBOARD_BRIDGE_HPP

#include <optional>

class Machine;
class QKeyEvent;

class MachineKeyboardBridge {
public:
    static std::optional<MachineKeyboardBridge> fromMachine(Machine& machine);

    void tick();
    bool handleSdlKeyboardEvent(const void* sdl_event, Machine* machine = nullptr);
    bool handleQtKeyEvent(QKeyEvent* event, Machine* machine = nullptr);

private:
    MachineKeyboardBridge() = default;
};

#endif
