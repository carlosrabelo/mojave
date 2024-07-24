#ifndef MOJAVE_SINCLAIR_TYPING_CHORD_HPP
#define MOJAVE_SINCLAIR_TYPING_CHORD_HPP

#include "devices/sinclair/keyboard.hpp"

struct SinclairTypingChord {
    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;
    bool shift = false;
};

bool sinclairTypingChordForChar(char ch, SinclairTypingChord& out);
bool sinclairTypingChordNeedsLetterMode(const SinclairTypingChord& chord);

#endif
