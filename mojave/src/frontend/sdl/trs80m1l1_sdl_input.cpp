#include "frontend/sdl/trs80m1l1_sdl_input.hpp"
#include "frontend/shared/trs80m1l1_host_input.hpp"
#include "frontend/shared/trs80m1l1_host_shortcuts.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "machines/shared/machine.hpp"
#include <SDL2/SDL.h>

namespace {

bool sdlModifierOnlyKey(SDL_Keycode sym) {
    switch (sym) {
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
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

bool sdlCtrlDown(const SDL_KeyboardEvent& key) {
    return (key.keysym.mod & KMOD_CTRL) != 0;
}

bool sdlLetterFromSym(SDL_Keycode sym, char& out) {
    if (sym >= SDLK_a && sym <= SDLK_z) {
        out = static_cast<char>(sym);
        return true;
    }
    if (sym >= 'A' && sym <= 'Z') {
        out = static_cast<char>(sym);
        return true;
    }
    return false;
}

bool sdlKeySymToSpecial(Trs80M1Keyboard::SpecialKey& out, SDL_Keycode sym) {
    switch (sym) {
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        out = Trs80M1Keyboard::SpecialKey::Enter;
        return true;
    case SDLK_BACKSPACE:
    case SDLK_LEFT:
        out = Trs80M1Keyboard::SpecialKey::Left;
        return true;
    case SDLK_UP:
        out = Trs80M1Keyboard::SpecialKey::Up;
        return true;
    case SDLK_DOWN:
        out = Trs80M1Keyboard::SpecialKey::Down;
        return true;
    case SDLK_RIGHT:
        out = Trs80M1Keyboard::SpecialKey::Right;
        return true;
    default:
        return false;
    }
}

} // namespace

bool trs80m1l1HandleSdlKeyboardEvent(Trs80M1L1HostKeyboardAdapter& adapter, const SDL_Event& event,
                                     Machine* machine) {
    if (event.type == SDL_TEXTINPUT) {
        trs80m1l1HostPulseHostText(adapter, event.text.text);
        return true;
    }

    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
        return false;

    const bool down = event.type == SDL_KEYDOWN;
    if (!down)
        return true;
    if (event.key.repeat != 0)
        return true;

    const SDL_Keycode sym = event.key.keysym.sym;
    if (sdlModifierOnlyKey(sym))
        return true;

    if (sdlUnusedHostModifier(event.key.keysym.mod))
        return true;

    if (sdlCtrlDown(event.key)) {
        if (!down)
            return true;
        char letter = '\0';
        Trs80M1Keyboard::SpecialKey special = Trs80M1Keyboard::SpecialKey::Enter;
        if (sdlLetterFromSym(sym, letter) && trs80m1l1HostCtrlShortcut(letter, special))
            trs80m1l1HostPulseSpecialKey(adapter, special, machine);
        return true;
    }

    Trs80M1Keyboard::SpecialKey special = Trs80M1Keyboard::SpecialKey::Enter;
    if (sdlKeySymToSpecial(special, sym)) {
        trs80m1l1HostPulseSpecialKey(adapter, special, machine);
        return true;
    }

    return true;
}
