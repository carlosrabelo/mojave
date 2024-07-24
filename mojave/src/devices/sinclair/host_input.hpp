#ifndef MOJAVE_SINCLAIR_HOST_INPUT_HPP
#define MOJAVE_SINCLAIR_HOST_INPUT_HPP

#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/host_keyboard_adapter.hpp"
#include "devices/sinclair/typing_chord.hpp"

void sinclairHostSyncShift(SinclairHostKeyboardAdapter& adapter, bool shift_down);
bool sinclairHostMatrixKeyDown(SinclairHostKeyboardAdapter& adapter, SinclairKeyboard::Key key);
void sinclairHostMatrixKeyUp(SinclairHostKeyboardAdapter& adapter, SinclairKeyboard::Key key);
void sinclairHostApplyTypingChord(SinclairHostKeyboardAdapter& adapter, const SinclairTypingChord& chord,
                                  bool down);
void sinclairHostPulseRubout(SinclairHostKeyboardAdapter& adapter);

#endif
