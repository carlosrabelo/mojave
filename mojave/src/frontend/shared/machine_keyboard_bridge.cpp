#include "frontend/shared/machine_keyboard_bridge.hpp"

MachineKeyboardBridge::MachineKeyboardBridge(Trs80M3HostKeyboardBridge bridge)
    : bridge_(std::move(bridge)) {}

MachineKeyboardBridge::MachineKeyboardBridge(Trs80HostKeyboardBridge bridge)
    : bridge_(std::move(bridge)) {}

MachineKeyboardBridge::MachineKeyboardBridge(Zx80HostKeyboardBridge bridge)
    : bridge_(std::move(bridge)) {}

std::optional<MachineKeyboardBridge> MachineKeyboardBridge::fromMachine(Machine& machine) {
    if (auto m3 = Trs80M3HostKeyboardBridge::fromMachine(machine))
        return MachineKeyboardBridge(std::move(*m3));
    if (auto m1 = Trs80HostKeyboardBridge::fromMachine(machine))
        return MachineKeyboardBridge(std::move(*m1));
    if (auto zx80 = Zx80HostKeyboardBridge::fromMachine(machine))
        return MachineKeyboardBridge(std::move(*zx80));
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
