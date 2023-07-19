#include "devices/trs80m1/keyboard.hpp"

namespace {

bool matchKey(char name, char expected, uint8_t row, uint8_t bit, uint8_t& out_row, uint8_t& out_bit) {
    if (name != expected)
        return false;
    out_row = row;
    out_bit = bit;
    return true;
}

} // namespace

uint8_t Trs80M1Keyboard::read(uint16_t address) {
    const uint8_t row_mask = static_cast<uint8_t>(address & 0xFFu);
    if (row_mask == 0)
        return 0x00;

    uint8_t value = 0;
    for (uint8_t row = 0; row < kRowCount; ++row) {
        if ((row_mask & (1u << row)) != 0)
            value = static_cast<uint8_t>(value | rows_[row]);
    }
    return value;
}

void Trs80M1Keyboard::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Trs80M1Keyboard::reset() {
    rows_.fill(0);
}

void Trs80M1Keyboard::setKeyDown(uint8_t row, uint8_t bit, bool down) {
    if (row >= kRowCount || bit >= kBitsPerRow)
        return;
    if (down)
        rows_[row] = static_cast<uint8_t>(rows_[row] | (1u << bit));
    else
        rows_[row] = static_cast<uint8_t>(rows_[row] & ~(1u << bit));
}

bool Trs80M1Keyboard::keyDown(uint8_t row, uint8_t bit) const {
    if (row >= kRowCount || bit >= kBitsPerRow)
        return false;
    return (rows_[row] & (1u << bit)) != 0;
}

void Trs80M1Keyboard::releaseAll() {
    rows_.fill(0);
}

bool Trs80M1Keyboard::lookupNamedKey(char name, uint8_t& row, uint8_t& bit) {
    if (name >= 'a' && name <= 'z')
        name = static_cast<char>(name - 'a' + 'A');
    if (matchKey(name, '@', 0, 0, row, bit))
        return true;
    if (name >= 'A' && name <= 'G')
        return matchKey(name, name, 0, static_cast<uint8_t>(1u + (name - 'A')), row, bit);
    if (name >= 'H' && name <= 'O')
        return matchKey(name, name, 1, static_cast<uint8_t>(name - 'H'), row, bit);
    if (name >= 'P' && name <= 'W')
        return matchKey(name, name, 2, static_cast<uint8_t>(name - 'P'), row, bit);
    if (name >= 'X' && name <= 'Z')
        return matchKey(name, name, 3, static_cast<uint8_t>(name - 'X'), row, bit);
    if (name >= '0' && name <= '7')
        return matchKey(name, name, 4, static_cast<uint8_t>(name - '0'), row, bit);
    if (name >= '8' && name <= '9')
        return matchKey(name, name, 5, static_cast<uint8_t>(name - '8'), row, bit);
    switch (name) {
    case ':':
        return matchKey(name, name, 5, 2, row, bit);
    case ';':
        return matchKey(name, name, 5, 3, row, bit);
    case ',':
        return matchKey(name, name, 5, 4, row, bit);
    case '-':
        return matchKey(name, name, 5, 5, row, bit);
    case '.':
        return matchKey(name, name, 5, 6, row, bit);
    case '/':
        return matchKey(name, name, 5, 7, row, bit);
    case '\r':
    case '\n':
        return matchKey(name, '\r', 6, 0, row, bit);
    case ' ':
        return matchKey(name, name, 6, 7, row, bit);
    default:
        break;
    }
    return false;
}

bool Trs80M1Keyboard::namedKeyBit(char name, uint8_t& row, uint8_t& bit) {
    return lookupNamedKey(name, row, bit);
}

bool Trs80M1Keyboard::specialKeyBit(SpecialKey key, uint8_t& row, uint8_t& bit) {
    switch (key) {
    case SpecialKey::Enter:
        row = 6;
        bit = 0;
        return true;
    case SpecialKey::Clear:
        row = 6;
        bit = 1;
        return true;
    case SpecialKey::Break:
        row = 6;
        bit = 2;
        return true;
    case SpecialKey::Up:
        row = 6;
        bit = 3;
        return true;
    case SpecialKey::Down:
        row = 6;
        bit = 4;
        return true;
    case SpecialKey::Left:
        row = 6;
        bit = 5;
        return true;
    case SpecialKey::Right:
        row = 6;
        bit = 6;
        return true;
    case SpecialKey::Shift:
        row = 7;
        bit = 0;
        return true;
    }
    return false;
}

bool Trs80M1Keyboard::pressNamedKey(char name) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!lookupNamedKey(name, row, bit))
        return false;
    setKeyDown(row, bit, true);
    return true;
}

void Trs80M1Keyboard::releaseNamedKey(char name) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!lookupNamedKey(name, row, bit))
        return;
    setKeyDown(row, bit, false);
}

void Trs80M1Keyboard::pressSpecialKey(SpecialKey key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!specialKeyBit(key, row, bit))
        return;
    setKeyDown(row, bit, true);
}

void Trs80M1Keyboard::releaseSpecialKey(SpecialKey key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!specialKeyBit(key, row, bit))
        return;
    setKeyDown(row, bit, false);
}
