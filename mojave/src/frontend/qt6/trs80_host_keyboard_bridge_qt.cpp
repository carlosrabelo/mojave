#if __has_include(<QKeyEvent>)
#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"
#include "frontend/qt6/trs80m1l1_qt_input.hpp"

bool Trs80HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* event, Machine* machine) {
    return trs80m1l1HandleQtKeyEvent(adapter_, event, machine);
}

#endif
