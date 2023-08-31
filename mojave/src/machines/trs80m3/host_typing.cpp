#include "machines/trs80m3/host_typing.hpp"
#include "machines/trs80m3/host_keyboard_adapter.hpp"
#include "machines/trs80m3/typing_chord.hpp"
#include <cstddef>
#include <cstdint>

namespace {

bool utf8FirstCodepoint(const char* text, uint32_t& codepoint, size_t& bytes) {
    if (text == nullptr || text[0] == '\0')
        return false;

    const unsigned char b0 = static_cast<unsigned char>(text[0]);
    if (b0 < 0x80) {
        codepoint = b0;
        bytes = 1;
        return true;
    }
    if ((b0 & 0xE0) == 0xC0 && text[1] != '\0') {
        const unsigned char b1 = static_cast<unsigned char>(text[1]);
        if ((b1 & 0xC0) != 0x80)
            return false;
        codepoint = ((b0 & 0x1Fu) << 6) | (b1 & 0x3Fu);
        bytes = 2;
        return true;
    }
    return false;
}

bool mapHostCodepoint(uint32_t codepoint, char& out) {
    switch (codepoint) {
    case 0x201C:
    case 0x201D:
        out = '"';
        return true;
    case 0x2018:
    case 0x2019:
        out = '\'';
        return true;
    default:
        break;
    }

    if (codepoint < 0x20 || codepoint > 0x7F)
        return false;
    out = static_cast<char>(codepoint);
    return true;
}

} // namespace

bool trs80m3HostCharFromText(const char* utf8, char& out) {
    uint32_t codepoint = 0;
    size_t bytes = 0;
    if (!utf8FirstCodepoint(utf8, codepoint, bytes))
        return false;
    if (utf8[bytes] != '\0')
        return false;
    return mapHostCodepoint(codepoint, out);
}

bool trs80m3HostPulseText(Trs80M3HostKeyboardAdapter& adapter, const char* utf8) {
    char trs80_ch = '\0';
    if (!trs80m3HostCharFromText(utf8, trs80_ch))
        return false;

    Trs80M3TypingChord chord;
    if (!trs80m3TypingChordForChar(trs80_ch, chord))
        return false;

    adapter.pulseCharacter(trs80_ch);
    return true;
}
