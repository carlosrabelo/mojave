#if __has_include(<QKeyEvent>)
#include "frontend/qt6/trs80m1l1_qt_input.hpp"
#include "frontend/shared/trs80m1l1_host_input.hpp"
#include "frontend/shared/trs80m1l1_host_shortcuts.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/shared/machine.hpp"
#include <QEvent>
#include <QKeyEvent>

namespace {

bool qtModifierOnlyKey(int key) {
    switch (key) {
    case Qt::Key_Shift:
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

bool qtKeyToSpecial(Trs80M1Keyboard::SpecialKey& out, int key) {
    switch (key) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        out = Trs80M1Keyboard::SpecialKey::Enter;
        return true;
    case Qt::Key_Backspace:
    case Qt::Key_Left:
        out = Trs80M1Keyboard::SpecialKey::Left;
        return true;
    case Qt::Key_Up:
        out = Trs80M1Keyboard::SpecialKey::Up;
        return true;
    case Qt::Key_Down:
        out = Trs80M1Keyboard::SpecialKey::Down;
        return true;
    case Qt::Key_Right:
        out = Trs80M1Keyboard::SpecialKey::Right;
        return true;
    default:
        return false;
    }
}

char qtLetterFromKey(int key) {
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        return static_cast<char>('A' + (key - Qt::Key_A));
    return '\0';
}

} // namespace

bool trs80m1l1HandleQtKeyEvent(Trs80M1L1HostKeyboardAdapter& adapter, QKeyEvent* event, Machine* machine) {
    if (!event)
        return false;

    const bool down = event->type() == QEvent::KeyPress;
    if (down && event->isAutoRepeat())
        return true;

    if (qtModifierOnlyKey(event->key()))
        return true;

    if (qtUnusedHostModifier(event->modifiers()))
        return true;

    if ((event->modifiers() & Qt::ControlModifier) != 0) {
        if (!down)
            return true;
        Trs80M1Keyboard::SpecialKey special = Trs80M1Keyboard::SpecialKey::Enter;
        const char letter = qtLetterFromKey(event->key());
        if (letter != '\0' && trs80m1l1HostCtrlShortcut(letter, special))
            trs80m1l1HostPulseSpecialKey(adapter, special, machine);
        return true;
    }

    Trs80M1Keyboard::SpecialKey special = Trs80M1Keyboard::SpecialKey::Enter;
    if (qtKeyToSpecial(special, event->key())) {
        if (down)
            trs80m1l1HostPulseSpecialKey(adapter, special, machine);
        return true;
    }

    if (!down)
        return true;

    const QByteArray text = event->text().toUtf8();
    if (!text.isEmpty())
        trs80m1l1HostPulseHostText(adapter, text.constData());

    return true;
}

#endif
