#if __has_include(<QKeyEvent>)
#include "frontend/shared/sinclair_qt_input.hpp"
#include "devices/sinclair/host_input.hpp"
#include "devices/sinclair/host_keymap.hpp"
#include "devices/sinclair/typing_chord.hpp"
#include <QEvent>
#include <QKeyEvent>

namespace {

bool qtShiftKey(int key) {
    return key == Qt::Key_Shift;
}

bool qtModifierOnlyKey(int key) {
    switch (key) {
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_Meta:
    case Qt::Key_AltGr:
    case Qt::Key_CapsLock:
    case Qt::Key_NumLock:
    case Qt::Key_ScrollLock:
        return true;
    default:
        return false;
    }
}

bool qtUnusedHostModifier(Qt::KeyboardModifiers mods) {
    return (mods & (Qt::AltModifier | Qt::MetaModifier)) != 0;
}

bool qtKeyToMatrixKey(int key, SinclairKeyboard::Key& out) {
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        return sinclairHostKeyFromLetter(static_cast<char>('A' + (key - Qt::Key_A)), out);
    if (key >= Qt::Key_0 && key <= Qt::Key_9)
        return sinclairHostKeyFromDigit(static_cast<char>('0' + (key - Qt::Key_0)), out);

    switch (key) {
    case Qt::Key_Space:
        out = SinclairKeyboard::Key::Space;
        return true;
    case Qt::Key_Period:
        out = SinclairKeyboard::Key::Dot;
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        out = SinclairKeyboard::Key::Enter;
        return true;
    default:
        return false;
    }
}

bool qtKeyToHostChar(int key, Qt::KeyboardModifiers mods, char& out) {
    const bool shift = (mods & Qt::ShiftModifier) != 0;

    switch (key) {
    case Qt::Key_QuoteDbl:
    case Qt::Key_Apostrophe:
        out = '"';
        return true;
    case Qt::Key_AsciiTilde:
        if (shift) {
            out = '"';
            return true;
        }
        return false;
    case Qt::Key_Semicolon:
    case Qt::Key_Colon:
        out = shift ? ':' : ';';
        return true;
    case Qt::Key_Comma:
    case Qt::Key_Less:
        out = shift ? '<' : ',';
        return true;
    case Qt::Key_Minus:
    case Qt::Key_Underscore:
        out = '-';
        return true;
    case Qt::Key_Equal:
    case Qt::Key_Plus:
        out = shift ? '+' : '=';
        return true;
    case Qt::Key_Slash:
    case Qt::Key_Question:
        out = shift ? '?' : '/';
        return true;
    case Qt::Key_ParenLeft:
        out = '(';
        return true;
    case Qt::Key_ParenRight:
        out = ')';
        return true;
    case Qt::Key_Asterisk:
        out = '*';
        return true;
    case Qt::Key_Dollar:
        out = '$';
        return true;
    default:
        return false;
    }
}

bool qtApplyHostChar(SinclairHostKeyboardAdapter& adapter, int key, Qt::KeyboardModifiers mods,
                     bool down) {
    char ch = '\0';
    if (!qtKeyToHostChar(key, mods, ch))
        return false;

    SinclairTypingChord chord;
    if (!sinclairTypingChordForChar(ch, chord))
        return false;

    sinclairHostApplyTypingChord(adapter, chord, down);
    return true;
}

void qtApplyMatrixKey(SinclairHostKeyboardAdapter& adapter, SinclairKeyboard::Key key,
                      Qt::KeyboardModifiers mods, bool down) {
    if (down) {
        if ((mods & Qt::ShiftModifier) != 0)
            sinclairHostSyncShift(adapter, true);
        else
            sinclairHostSyncShift(adapter, false);
        sinclairHostMatrixKeyDown(adapter, key);
        return;
    }

    sinclairHostMatrixKeyUp(adapter, key);
    if ((mods & Qt::ShiftModifier) == 0)
        sinclairHostSyncShift(adapter, false);
}

} // namespace

bool sinclairHandleQtKeyEvent(SinclairHostKeyboardAdapter& adapter, QKeyEvent* event, Machine* /*machine*/) {
    if (!event)
        return false;

    const bool down = event->type() == QEvent::KeyPress;
    if (down && event->isAutoRepeat())
        return true;

    if (qtUnusedHostModifier(event->modifiers()))
        return true;

    if (qtShiftKey(event->key())) {
        sinclairHostSyncShift(adapter, down);
        return true;
    }

    if (qtModifierOnlyKey(event->key()))
        return true;

    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        if (down)
            sinclairHostPulseRubout(adapter);
        return true;
    }

    if (qtApplyHostChar(adapter, event->key(), event->modifiers(), down))
        return true;

    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;
    if (qtKeyToMatrixKey(event->key(), key)) {
        qtApplyMatrixKey(adapter, key, event->modifiers(), down);
        return true;
    }

    return true;
}

#endif
