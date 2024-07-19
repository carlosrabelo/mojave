#include "frontend/shared/sinclair_sdl_input.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "devices/sinclair/host_keymap.hpp"

namespace {

bool sdlShiftKey(SDL_Keycode sym) {
    return sym == SDLK_LSHIFT || sym == SDLK_RSHIFT;
}

bool sdlModifierOnlyKey(SDL_Keycode sym) {
    switch (sym) {
    case SDLK_LCTRL:
    case SDLK_RCTRL:
    case SDLK_LALT:
    case SDLK_RALT:
    case SDLK_LGUI:
    case SDLK_RGUI:
    case SDLK_MODE:
    case SDLK_CAPSLOCK:
    case SDLK_NUMLOCKCLEAR:
    case SDLK_SCROLLLOCK:
        return true;
    default:
        return false;
    }
}

bool sdlUnusedHostModifier(uint16_t mod) {
    return (mod & (KMOD_ALT | KMOD_GUI)) != 0;
}

bool sdlSymToMatrixKey(SDL_Keycode sym, SinclairKeyboard::Key& out) {
    if (sym >= SDLK_a && sym <= SDLK_z)
        return sinclairHostKeyFromLetter(static_cast<char>('a' + (sym - SDLK_a)), out);
    if (sym >= SDLK_0 && sym <= SDLK_9)
        return sinclairHostKeyFromDigit(static_cast<char>(sym), out);

    switch (sym) {
    case SDLK_SPACE:
        out = SinclairKeyboard::Key::Space;
        return true;
    case SDLK_PERIOD:
    case SDLK_KP_PERIOD:
        out = SinclairKeyboard::Key::Dot;
        return true;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        out = SinclairKeyboard::Key::Enter;
        return true;
    case SDLK_KP_0:
        out = SinclairKeyboard::Key::Digit0;
        return true;
    case SDLK_KP_1:
        out = SinclairKeyboard::Key::Digit1;
        return true;
    case SDLK_KP_2:
        out = SinclairKeyboard::Key::Digit2;
        return true;
    case SDLK_KP_3:
        out = SinclairKeyboard::Key::Digit3;
        return true;
    case SDLK_KP_4:
        out = SinclairKeyboard::Key::Digit4;
        return true;
    case SDLK_KP_5:
        out = SinclairKeyboard::Key::Digit5;
        return true;
    case SDLK_KP_6:
        out = SinclairKeyboard::Key::Digit6;
        return true;
    case SDLK_KP_7:
        out = SinclairKeyboard::Key::Digit7;
        return true;
    case SDLK_KP_8:
        out = SinclairKeyboard::Key::Digit8;
        return true;
    case SDLK_KP_9:
        out = SinclairKeyboard::Key::Digit9;
        return true;
    default:
        return false;
    }
}

} // namespace

bool sinclairHandleSdlKeyboardEvent(SinclairHostKeyboardAdapter& adapter, const SDL_Event& event,
                                    Machine* /*machine*/) {
    if (event.type == SDL_TEXTINPUT)
        return true;

    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
        return false;

    const bool down = event.type == SDL_KEYDOWN;
    if (down && event.key.repeat != 0)
        return true;

    const SDL_Keycode sym = event.key.keysym.sym;
    const uint16_t mod = event.key.keysym.mod;

    if (sdlUnusedHostModifier(mod))
        return true;

    if (sdlShiftKey(sym)) {
        if (down)
            adapter.hostKeyDown(SinclairKeyboard::Key::Shift);
        else
            adapter.hostKeyUp(SinclairKeyboard::Key::Shift);
        return true;
    }

    if (sdlModifierOnlyKey(sym))
        return true;

    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;
    if (!sdlSymToMatrixKey(sym, key))
        return true;

    if (down)
        adapter.hostKeyDown(key);
    else
        adapter.hostKeyUp(key);
    return true;
}
