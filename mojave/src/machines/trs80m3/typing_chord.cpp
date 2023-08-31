#include "machines/trs80m3/typing_chord.hpp"
#include "devices/trs80m3/keyboard.hpp"

namespace {

bool unshiftedNamed(char name, Trs80M3TypingChord& out) {
    if (!Trs80M3Keyboard::namedKeyBit(name, out.row, out.bit))
        return false;
    out.shift = false;
    return true;
}

bool shiftedKey(uint8_t row, uint8_t bit, Trs80M3TypingChord& out) {
    out.row = row;
    out.bit = bit;
    out.shift = true;
    return true;
}

} // namespace

bool trs80m3TypingChordForChar(char ch, Trs80M3TypingChord& out) {
    if (ch >= 'a' && ch <= 'z')
        ch = static_cast<char>(ch - 'a' + 'A');

    if (unshiftedNamed(ch, out))
        return true;

    switch (ch) {
    case '!':
        return shiftedKey(4, 1, out);
    case '"':
        return shiftedKey(4, 2, out);
    case '#':
        return shiftedKey(4, 3, out);
    case '$':
        return shiftedKey(4, 4, out);
    case '%':
        return shiftedKey(4, 5, out);
    case '&':
        return shiftedKey(4, 6, out);
    case '\'':
        return shiftedKey(4, 7, out);
    case '(':
        return shiftedKey(5, 0, out);
    case ')':
        return shiftedKey(5, 1, out);
    case '*':
        return shiftedKey(5, 2, out);
    case '+':
        return shiftedKey(5, 3, out);
    case '<':
        return shiftedKey(5, 4, out);
    case '=':
        return shiftedKey(5, 5, out);
    case '>':
        return shiftedKey(5, 6, out);
    case '?':
        return shiftedKey(5, 7, out);
    default:
        return false;
    }
}
