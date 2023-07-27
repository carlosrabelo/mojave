#include "machines/trs80m1l1/trs80_host_keyboard_bridge.hpp"
#include "frontend/sdl/trs80m1l1_sdl_input.hpp"

bool Trs80HostKeyboardBridge::handleSdlKeyboardEvent(const void* sdl_event, Machine* machine) {
    return trs80m1l1HandleSdlKeyboardEvent(adapter_, *static_cast<const SDL_Event*>(sdl_event), machine);
}
