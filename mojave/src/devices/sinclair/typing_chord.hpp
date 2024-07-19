#ifndef MOJAVE_SINCLAIR_TYPING_CHORD_HPP
#define MOJAVE_SINCLAIR_TYPING_CHORD_HPP

#include "devices/sinclair/keyboard.hpp"

// Shift + matrix key held together. Host-layout symbol mapping is added in the
// later typing TODO item.
struct SinclairTypingChord {
    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;
    bool shift = false;
};

#endif
