#if __has_include(<QKeyEvent>)
#include "machines/trs80m3/trs80_host_keyboard_bridge.hpp"
#include "frontend/qt6/trs80m3_qt_input.hpp"

bool Trs80M3HostKeyboardBridge::handleQtKeyEvent(QKeyEvent* event, Machine* machine) {
    return trs80m3HandleQtKeyEvent(adapter_, event, machine);
}

#endif
