#include "machines/zx80/zx80_host_keyboard_bridge.hpp"
#include "frontend/shared/sinclair_sdl_input.hpp"

bool Zx80HostKeyboardBridge::handleSdlKeyboardEvent(const void* sdl_event, Machine* machine) {
    return sinclairHandleSdlKeyboardEvent(adapter_, *static_cast<const SDL_Event*>(sdl_event), machine);
}
