#ifndef MOJAVE_TRS80M3_KEYBOARD_HPP
#define MOJAVE_TRS80M3_KEYBOARD_HPP

#include <array>
#include <cstdint>
#include "devices/device.hpp"

// Self-contained TRS-80 Model III keyboard matrix.
//
// This device intentionally does NOT share state with the Model I keyboard:
// the Model III matrix is a separate piece of hardware (it adds CONTROL and
// CAPS LOCK on row 7). Keeping it isolated means changes to the Model III
// keyboard can never affect the Model I presets and vice versa. The letter,
// digit and symbol mapping is replicated here on purpose.
class Trs80M3Keyboard : public Device {
public:
    static constexpr uint16_t kMatrixBase = 0x3800;
    static constexpr uint8_t kRowCount = 8;
    static constexpr uint8_t kBitsPerRow = 8;

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    static constexpr uint16_t rowAddress(uint8_t row) {
        return static_cast<uint16_t>(kMatrixBase | (1u << row));
    }

    static constexpr uint16_t rowOffset(uint8_t row) {
        return static_cast<uint16_t>(rowAddress(row) - kMatrixBase);
    }

    void setKeyDown(uint8_t row, uint8_t bit, bool down);
    bool keyDown(uint8_t row, uint8_t bit) const;
    void releaseAll();

    enum class SpecialKey : uint8_t {
        Enter,
        Clear,
        Break,
        Up,
        Down,
        Left,
        Right,
        Shift,
        Control,
        CapsLock,
    };

    static bool namedKeyBit(char name, uint8_t& row, uint8_t& bit);
    static bool specialKeyBit(SpecialKey key, uint8_t& row, uint8_t& bit);

    bool pressNamedKey(char name);
    void releaseNamedKey(char name);

    void pressSpecialKey(SpecialKey key);
    void releaseSpecialKey(SpecialKey key);

private:
    std::array<uint8_t, kRowCount> rows_{};

    static bool lookupNamedKey(char name, uint8_t& row, uint8_t& bit);
};

#endif
