#include "devices/sinclair/keyboard.hpp"

namespace {

bool matchKey(SinclairKeyboard::Key key, SinclairKeyboard::Key expected, uint8_t row, uint8_t bit,
              uint8_t& out_row, uint8_t& out_bit) {
    if (key != expected)
        return false;
    out_row = row;
    out_bit = bit;
    return true;
}

} // namespace

std::optional<uint8_t> SinclairKeyboard::rowFromPort(uint16_t port) {
    if ((port & 0x0001u) != 0)
        return std::nullopt;

    const uint8_t high = static_cast<uint8_t>((port >> 8) & 0xFFu);
    uint8_t selected = 0;
    uint8_t count = 0;
    for (uint8_t row = 0; row < kRowCount; ++row) {
        if ((high & static_cast<uint8_t>(1u << row)) != 0)
            continue;
        selected = row;
        ++count;
    }
    if (count != 1)
        return std::nullopt;
    return selected;
}

bool SinclairKeyboard::keyBit(Key key, uint8_t& row, uint8_t& bit) {
    if (matchKey(key, Key::Shift, 0, 0, row, bit))
        return true;
    if (matchKey(key, Key::Z, 0, 1, row, bit))
        return true;
    if (matchKey(key, Key::X, 0, 2, row, bit))
        return true;
    if (matchKey(key, Key::C, 0, 3, row, bit))
        return true;
    if (matchKey(key, Key::V, 0, 4, row, bit))
        return true;

    if (matchKey(key, Key::A, 1, 0, row, bit))
        return true;
    if (matchKey(key, Key::S, 1, 1, row, bit))
        return true;
    if (matchKey(key, Key::D, 1, 2, row, bit))
        return true;
    if (matchKey(key, Key::F, 1, 3, row, bit))
        return true;
    if (matchKey(key, Key::G, 1, 4, row, bit))
        return true;

    if (matchKey(key, Key::Q, 2, 0, row, bit))
        return true;
    if (matchKey(key, Key::W, 2, 1, row, bit))
        return true;
    if (matchKey(key, Key::E, 2, 2, row, bit))
        return true;
    if (matchKey(key, Key::R, 2, 3, row, bit))
        return true;
    if (matchKey(key, Key::T, 2, 4, row, bit))
        return true;

    if (matchKey(key, Key::Digit1, 3, 0, row, bit))
        return true;
    if (matchKey(key, Key::Digit2, 3, 1, row, bit))
        return true;
    if (matchKey(key, Key::Digit3, 3, 2, row, bit))
        return true;
    if (matchKey(key, Key::Digit4, 3, 3, row, bit))
        return true;
    if (matchKey(key, Key::Digit5, 3, 4, row, bit))
        return true;

    if (matchKey(key, Key::Digit0, 4, 0, row, bit))
        return true;
    if (matchKey(key, Key::Digit9, 4, 1, row, bit))
        return true;
    if (matchKey(key, Key::Digit8, 4, 2, row, bit))
        return true;
    if (matchKey(key, Key::Digit7, 4, 3, row, bit))
        return true;
    if (matchKey(key, Key::Digit6, 4, 4, row, bit))
        return true;

    if (matchKey(key, Key::P, 5, 0, row, bit))
        return true;
    if (matchKey(key, Key::O, 5, 1, row, bit))
        return true;
    if (matchKey(key, Key::I, 5, 2, row, bit))
        return true;
    if (matchKey(key, Key::U, 5, 3, row, bit))
        return true;
    if (matchKey(key, Key::Y, 5, 4, row, bit))
        return true;

    if (matchKey(key, Key::Enter, 6, 0, row, bit))
        return true;
    if (matchKey(key, Key::L, 6, 1, row, bit))
        return true;
    if (matchKey(key, Key::K, 6, 2, row, bit))
        return true;
    if (matchKey(key, Key::J, 6, 3, row, bit))
        return true;
    if (matchKey(key, Key::H, 6, 4, row, bit))
        return true;

    if (matchKey(key, Key::Space, 7, 0, row, bit))
        return true;
    if (matchKey(key, Key::Dot, 7, 1, row, bit))
        return true;
    if (matchKey(key, Key::M, 7, 2, row, bit))
        return true;
    if (matchKey(key, Key::N, 7, 3, row, bit))
        return true;
    if (matchKey(key, Key::B, 7, 4, row, bit))
        return true;

    return false;
}

uint8_t SinclairKeyboard::read(uint16_t /*address*/) {
    return 0xFF;
}

void SinclairKeyboard::write(uint16_t /*address*/, uint8_t /*value*/) {}

void SinclairKeyboard::reset() {
    rows_.fill(0);
}

uint8_t SinclairKeyboard::readPort(uint16_t port) {
    const auto row = rowFromPort(port);
    if (!row.has_value())
        return 0xFF;

    const uint8_t pressed = static_cast<uint8_t>(rows_[*row] & kKeyDataMask);
    return static_cast<uint8_t>((~pressed & kKeyDataMask) | kFloatingBitsMask);
}

void SinclairKeyboard::writePort(uint16_t /*port*/, uint8_t /*value*/) {}

void SinclairKeyboard::setKeyDown(uint8_t row, uint8_t bit, bool down) {
    if (row >= kRowCount || bit >= kBitsPerRow)
        return;
    if (down)
        rows_[row] = static_cast<uint8_t>(rows_[row] | (1u << bit));
    else
        rows_[row] = static_cast<uint8_t>(rows_[row] & ~(1u << bit));
}

bool SinclairKeyboard::keyDown(uint8_t row, uint8_t bit) const {
    if (row >= kRowCount || bit >= kBitsPerRow)
        return false;
    return (rows_[row] & (1u << bit)) != 0;
}

void SinclairKeyboard::releaseAll() {
    rows_.fill(0);
}

void SinclairKeyboard::pressKey(Key key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!keyBit(key, row, bit))
        return;
    setKeyDown(row, bit, true);
}

void SinclairKeyboard::releaseKey(Key key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!keyBit(key, row, bit))
        return;
    setKeyDown(row, bit, false);
}
