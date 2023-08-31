#ifndef MOJAVE_TRS80M3_TYPING_CHORD_HPP
#define MOJAVE_TRS80M3_TYPING_CHORD_HPP

#include <cstdint>

struct Trs80M3TypingChord {
    uint8_t row = 0;
    uint8_t bit = 0;
    bool shift = false;
};

bool trs80m3TypingChordForChar(char ch, Trs80M3TypingChord& out);

#endif
