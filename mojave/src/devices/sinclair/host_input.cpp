#include "devices/sinclair/host_input.hpp"

void sinclairHostSyncShift(SinclairHostKeyboardAdapter& adapter, bool shift_down) {
    if (shift_down)
        adapter.hostKeyDown(SinclairKeyboard::Key::Shift);
    else
        adapter.hostKeyUp(SinclairKeyboard::Key::Shift);
}

bool sinclairHostMatrixKeyDown(SinclairHostKeyboardAdapter& adapter, SinclairKeyboard::Key key) {
    return adapter.hostKeyDown(key);
}

void sinclairHostMatrixKeyUp(SinclairHostKeyboardAdapter& adapter, SinclairKeyboard::Key key) {
    adapter.hostKeyUp(key);
}

void sinclairHostApplyTypingChord(SinclairHostKeyboardAdapter& adapter, const SinclairTypingChord& chord,
                                  bool down) {
    if (down) {
        if (chord.shift)
            sinclairHostSyncShift(adapter, true);
        sinclairHostMatrixKeyDown(adapter, chord.key);
        return;
    }

    sinclairHostMatrixKeyUp(adapter, chord.key);
    if (chord.shift)
        sinclairHostSyncShift(adapter, false);
}

void sinclairHostPulseRubout(SinclairHostKeyboardAdapter& adapter) {
    adapter.pulseKey(SinclairKeyboard::Key::Digit0, true);
}
