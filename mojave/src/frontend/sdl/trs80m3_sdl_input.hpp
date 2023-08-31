#ifndef MOJAVE_TRS80M3_SDL_INPUT_HPP
#define MOJAVE_TRS80M3_SDL_INPUT_HPP

#include <SDL2/SDL.h>

class Machine;
class Trs80M3HostKeyboardAdapter;

bool trs80m3HandleSdlKeyboardEvent(Trs80M3HostKeyboardAdapter& adapter, const SDL_Event& event,
                                   Machine* machine = nullptr);

#endif
