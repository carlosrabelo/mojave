#ifndef MOJAVE_SINCLAIR_HOST_KEYMAP_HPP
#define MOJAVE_SINCLAIR_HOST_KEYMAP_HPP

#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/typing_chord.hpp"

bool sinclairHostKeyFromLetter(char ch, SinclairKeyboard::Key& out);
bool sinclairHostKeyFromDigit(char ch, SinclairKeyboard::Key& out);
bool sinclairHostTypingChordForHostChar(char ch, SinclairTypingChord& out);

#endif
