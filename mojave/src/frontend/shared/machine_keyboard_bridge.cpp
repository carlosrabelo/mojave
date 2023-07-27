#include "frontend/shared/machine_keyboard_bridge.hpp"

MachineKeyboardBridge::MachineKeyboardBridge(Trs80HostKeyboardBridge bridge)
    : bridge_(std::move(bridge)) {}

std::optional<MachineKeyboardBridge> MachineKeyboardBridge::fromMachine(Machine& machine) {
    if (auto m1 = Trs80HostKeyboardBridge::fromMachine(machine))
        return MachineKeyboardBridge(std::move(*m1));
    return std::nullopt;
}

void MachineKeyboardBridge::tick() {
    std::visit([](auto& bridge) { bridge.tick(); }, bridge_);
}

bool MachineKeyboardBridge::handleSdlKeyboardEvent(const void* sdl_event, Machine* machine) {
    return std::visit([&](auto& bridge) { return bridge.handleSdlKeyboardEvent(sdl_event, machine); },
                      bridge_);
}

bool MachineKeyboardBridge::handleQtKeyEvent(QKeyEvent* event, Machine* machine) {
    return std::visit([&](auto& bridge) { return bridge.handleQtKeyEvent(event, machine); }, bridge_);
}
