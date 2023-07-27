#ifndef MOJAVE_TRS80M1L1_SDL_INPUT_HPP
#define MOJAVE_TRS80M1L1_SDL_INPUT_HPP

#include <SDL2/SDL.h>
class Trs80M1L1HostKeyboardAdapter;
class Machine;

bool trs80m1l1HandleSdlKeyboardEvent(Trs80M1L1HostKeyboardAdapter& adapter, const SDL_Event& event,
                                     Machine* machine = nullptr);

#endif
