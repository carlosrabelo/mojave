#include "machines/trs80m3/trs80_host_keyboard_bridge.hpp"
#include "frontend/sdl/trs80m3_sdl_input.hpp"
#include <SDL2/SDL.h>

bool Trs80M3HostKeyboardBridge::handleSdlKeyboardEvent(const void* sdl_event, Machine* machine) {
    return trs80m3HandleSdlKeyboardEvent(adapter_, *static_cast<const SDL_Event*>(sdl_event), machine);
}
