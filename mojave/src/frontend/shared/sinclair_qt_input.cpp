#if __has_include(<QKeyEvent>)
#include "frontend/shared/sinclair_qt_input.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "devices/sinclair/host_keymap.hpp"
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
        if (down)
            adapter.hostKeyDown(SinclairKeyboard::Key::Shift);
        else
            adapter.hostKeyUp(SinclairKeyboard::Key::Shift);
        return true;
    }

    if (qtModifierOnlyKey(event->key()))
        return true;

    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;
    if (!qtKeyToMatrixKey(event->key(), key))
        return true;

    if (down)
        adapter.hostKeyDown(key);
    else
        adapter.hostKeyUp(key);
    return true;
}

#endif
