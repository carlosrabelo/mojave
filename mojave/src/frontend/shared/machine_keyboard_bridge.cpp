#include "frontend/shared/machine_keyboard_bridge.hpp"

std::optional<MachineKeyboardBridge> MachineKeyboardBridge::fromMachine(Machine&) {
    return std::nullopt;
}

void MachineKeyboardBridge::tick() {}

bool MachineKeyboardBridge::handleSdlKeyboardEvent(const void*, Machine*) {
    return false;
}

bool MachineKeyboardBridge::handleQtKeyEvent(QKeyEvent*, Machine*) {
    return false;
}
