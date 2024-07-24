#include "devices/sinclair/host_keymap.hpp"

bool sinclairHostKeyFromLetter(char ch, SinclairKeyboard::Key& out) {
    if (ch >= 'a' && ch <= 'z')
        ch = static_cast<char>(ch - 'a' + 'A');

    switch (ch) {
    case 'A':
        out = SinclairKeyboard::Key::A;
        return true;
    case 'B':
        out = SinclairKeyboard::Key::B;
        return true;
    case 'C':
        out = SinclairKeyboard::Key::C;
        return true;
    case 'D':
        out = SinclairKeyboard::Key::D;
        return true;
    case 'E':
        out = SinclairKeyboard::Key::E;
        return true;
    case 'F':
        out = SinclairKeyboard::Key::F;
        return true;
    case 'G':
        out = SinclairKeyboard::Key::G;
        return true;
    case 'H':
        out = SinclairKeyboard::Key::H;
        return true;
    case 'I':
        out = SinclairKeyboard::Key::I;
        return true;
    case 'J':
        out = SinclairKeyboard::Key::J;
        return true;
    case 'K':
        out = SinclairKeyboard::Key::K;
        return true;
    case 'L':
        out = SinclairKeyboard::Key::L;
        return true;
    case 'M':
        out = SinclairKeyboard::Key::M;
        return true;
    case 'N':
        out = SinclairKeyboard::Key::N;
        return true;
    case 'O':
        out = SinclairKeyboard::Key::O;
        return true;
    case 'P':
        out = SinclairKeyboard::Key::P;
        return true;
    case 'Q':
        out = SinclairKeyboard::Key::Q;
        return true;
    case 'R':
        out = SinclairKeyboard::Key::R;
        return true;
    case 'S':
        out = SinclairKeyboard::Key::S;
        return true;
    case 'T':
        out = SinclairKeyboard::Key::T;
        return true;
    case 'U':
        out = SinclairKeyboard::Key::U;
        return true;
    case 'V':
        out = SinclairKeyboard::Key::V;
        return true;
    case 'W':
        out = SinclairKeyboard::Key::W;
        return true;
    case 'X':
        out = SinclairKeyboard::Key::X;
        return true;
    case 'Y':
        out = SinclairKeyboard::Key::Y;
        return true;
    case 'Z':
        out = SinclairKeyboard::Key::Z;
        return true;
    default:
        return false;
    }
}

bool sinclairHostKeyFromDigit(char ch, SinclairKeyboard::Key& out) {
    switch (ch) {
    case '0':
        out = SinclairKeyboard::Key::Digit0;
        return true;
    case '1':
        out = SinclairKeyboard::Key::Digit1;
        return true;
    case '2':
        out = SinclairKeyboard::Key::Digit2;
        return true;
    case '3':
        out = SinclairKeyboard::Key::Digit3;
        return true;
    case '4':
        out = SinclairKeyboard::Key::Digit4;
        return true;
    case '5':
        out = SinclairKeyboard::Key::Digit5;
        return true;
    case '6':
        out = SinclairKeyboard::Key::Digit6;
        return true;
    case '7':
        out = SinclairKeyboard::Key::Digit7;
        return true;
    case '8':
        out = SinclairKeyboard::Key::Digit8;
        return true;
    case '9':
        out = SinclairKeyboard::Key::Digit9;
        return true;
    default:
        return false;
    }
}

bool sinclairHostTypingChordForHostChar(char ch, SinclairTypingChord& out) {
    if (sinclairHostKeyFromLetter(ch, out.key)) {
        out.shift = false;
        return true;
    }
    if (sinclairHostKeyFromDigit(ch, out.key)) {
        out.shift = false;
        return true;
    }
    if (ch == ' ') {
        out.key = SinclairKeyboard::Key::Space;
        out.shift = false;
        return true;
    }
    if (ch == '.') {
        out.key = SinclairKeyboard::Key::Dot;
        out.shift = false;
        return true;
    }
    return sinclairTypingChordForChar(ch, out);
}
