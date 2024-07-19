#ifndef MOJAVE_SINCLAIR_SDL_INPUT_HPP
#define MOJAVE_SINCLAIR_SDL_INPUT_HPP

#include <SDL2/SDL.h>

class Machine;
class SinclairHostKeyboardAdapter;

bool sinclairHandleSdlKeyboardEvent(SinclairHostKeyboardAdapter& adapter, const SDL_Event& event,
                                    Machine* machine = nullptr);

#endif
