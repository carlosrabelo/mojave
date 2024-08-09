#if __has_include(<QKeyEvent>)
#include "machines/zx81/zx81_host_keyboard_bridge.hpp"
#include "frontend/shared/sinclair_qt_input.hpp"

bool Zx81HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* event, Machine* machine) {
    return sinclairHandleQtKeyEvent(adapter_, event, machine);
}

#endif
