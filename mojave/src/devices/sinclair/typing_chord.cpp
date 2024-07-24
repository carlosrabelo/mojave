#include "devices/sinclair/typing_chord.hpp"

namespace {

bool unshifted(SinclairKeyboard::Key key, SinclairTypingChord& out) {
    out.key = key;
    out.shift = false;
    return true;
}

bool shifted(SinclairKeyboard::Key key, SinclairTypingChord& out) {
    out.key = key;
    out.shift = true;
    return true;
}

} // namespace

bool sinclairTypingChordForChar(char ch, SinclairTypingChord& out) {
    if (ch >= 'a' && ch <= 'z')
        ch = static_cast<char>(ch - 'a' + 'A');

    switch (ch) {
    case 'A':
        return unshifted(SinclairKeyboard::Key::A, out);
    case 'B':
        return unshifted(SinclairKeyboard::Key::B, out);
    case 'C':
        return unshifted(SinclairKeyboard::Key::C, out);
    case 'D':
        return unshifted(SinclairKeyboard::Key::D, out);
    case 'E':
        return unshifted(SinclairKeyboard::Key::E, out);
    case 'F':
        return unshifted(SinclairKeyboard::Key::F, out);
    case 'G':
        return unshifted(SinclairKeyboard::Key::G, out);
    case 'H':
        return unshifted(SinclairKeyboard::Key::H, out);
    case 'I':
        return unshifted(SinclairKeyboard::Key::I, out);
    case 'J':
        return unshifted(SinclairKeyboard::Key::J, out);
    case 'K':
        return unshifted(SinclairKeyboard::Key::K, out);
    case 'L':
        return unshifted(SinclairKeyboard::Key::L, out);
    case 'M':
        return unshifted(SinclairKeyboard::Key::M, out);
    case 'N':
        return unshifted(SinclairKeyboard::Key::N, out);
    case 'O':
        return unshifted(SinclairKeyboard::Key::O, out);
    case 'P':
        return unshifted(SinclairKeyboard::Key::P, out);
    case 'Q':
        return unshifted(SinclairKeyboard::Key::Q, out);
    case 'R':
        return unshifted(SinclairKeyboard::Key::R, out);
    case 'S':
        return unshifted(SinclairKeyboard::Key::S, out);
    case 'T':
        return unshifted(SinclairKeyboard::Key::T, out);
    case 'U':
        return unshifted(SinclairKeyboard::Key::U, out);
    case 'V':
        return unshifted(SinclairKeyboard::Key::V, out);
    case 'W':
        return unshifted(SinclairKeyboard::Key::W, out);
    case 'X':
        return unshifted(SinclairKeyboard::Key::X, out);
    case 'Y':
        return unshifted(SinclairKeyboard::Key::Y, out);
    case 'Z':
        return unshifted(SinclairKeyboard::Key::Z, out);
    case '0':
        return unshifted(SinclairKeyboard::Key::Digit0, out);
    case '1':
        return unshifted(SinclairKeyboard::Key::Digit1, out);
    case '2':
        return unshifted(SinclairKeyboard::Key::Digit2, out);
    case '3':
        return unshifted(SinclairKeyboard::Key::Digit3, out);
    case '4':
        return unshifted(SinclairKeyboard::Key::Digit4, out);
    case '5':
        return unshifted(SinclairKeyboard::Key::Digit5, out);
    case '6':
        return unshifted(SinclairKeyboard::Key::Digit6, out);
    case '7':
        return unshifted(SinclairKeyboard::Key::Digit7, out);
    case '8':
        return unshifted(SinclairKeyboard::Key::Digit8, out);
    case '9':
        return unshifted(SinclairKeyboard::Key::Digit9, out);
    case ' ':
        return unshifted(SinclairKeyboard::Key::Space, out);
    case '.':
        return unshifted(SinclairKeyboard::Key::Dot, out);
    // Sinclair shift legends (ZX-80/ZX-81 membrane; shared chord map)
    case '"':
        return shifted(SinclairKeyboard::Key::P, out);
    case '$':
        return shifted(SinclairKeyboard::Key::U, out);
    case '(':
        return shifted(SinclairKeyboard::Key::I, out);
    case ')':
        return shifted(SinclairKeyboard::Key::O, out);
    case '-':
        return shifted(SinclairKeyboard::Key::J, out);
    case '+':
        return shifted(SinclairKeyboard::Key::K, out);
    case '=':
        return shifted(SinclairKeyboard::Key::L, out);
    case '<':
        return shifted(SinclairKeyboard::Key::N, out);
    case '>':
        return shifted(SinclairKeyboard::Key::M, out);
    case ',':
        return shifted(SinclairKeyboard::Key::Dot, out);
    case '/':
        return shifted(SinclairKeyboard::Key::V, out);
    case ':':
        return shifted(SinclairKeyboard::Key::Z, out);
    case ';':
        return shifted(SinclairKeyboard::Key::X, out);
    case '*':
        return shifted(SinclairKeyboard::Key::B, out);
    case '?':
        return shifted(SinclairKeyboard::Key::C, out);
    default:
        return false;
    }
}

bool sinclairTypingChordNeedsLetterMode(const SinclairTypingChord& chord) {
    if (chord.shift)
        return false;

    switch (chord.key) {
    case SinclairKeyboard::Key::A:
    case SinclairKeyboard::Key::B:
    case SinclairKeyboard::Key::C:
    case SinclairKeyboard::Key::D:
    case SinclairKeyboard::Key::E:
    case SinclairKeyboard::Key::F:
    case SinclairKeyboard::Key::G:
    case SinclairKeyboard::Key::H:
    case SinclairKeyboard::Key::I:
    case SinclairKeyboard::Key::J:
    case SinclairKeyboard::Key::K:
    case SinclairKeyboard::Key::L:
    case SinclairKeyboard::Key::M:
    case SinclairKeyboard::Key::N:
    case SinclairKeyboard::Key::O:
    case SinclairKeyboard::Key::P:
    case SinclairKeyboard::Key::Q:
    case SinclairKeyboard::Key::R:
    case SinclairKeyboard::Key::S:
    case SinclairKeyboard::Key::T:
    case SinclairKeyboard::Key::U:
    case SinclairKeyboard::Key::V:
    case SinclairKeyboard::Key::W:
    case SinclairKeyboard::Key::X:
    case SinclairKeyboard::Key::Y:
    case SinclairKeyboard::Key::Z:
        return true;
    default:
        return false;
    }
}
