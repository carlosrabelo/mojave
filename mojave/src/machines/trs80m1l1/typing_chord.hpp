#ifndef MOJAVE_TRS80M1L1_TYPING_CHORD_HPP
#define MOJAVE_TRS80M1L1_TYPING_CHORD_HPP

#include <cstdint>

struct Trs80M1L1TypingChord {
    uint8_t row = 0;
    uint8_t bit = 0;
    bool shift = false;
};

bool trs80m1l1TypingChordForChar(char ch, Trs80M1L1TypingChord& out);

#endif
