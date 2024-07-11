#ifndef MOJAVE_SINCLAIR_KEYBOARD_HPP
#define MOJAVE_SINCLAIR_KEYBOARD_HPP

#include <array>
#include <cstdint>
#include <optional>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

// Sinclair ZX-80/ZX-81 40-key membrane matrix (8 rows × 5 columns).
//
// Row select: IN with port low byte 0xFE (A0 low) and exactly one of A8–A15 low
// in the high byte (BC register during IN r,(C)). Five data bits D0–D4 read
// active-low (0 = pressed). Layout matches the ROM keyboard scan at 0x0143.
class SinclairKeyboard : public Device, public PortDevice {
public:
    static constexpr uint16_t kPortLowByte = 0x00FE;
    static constexpr uint8_t kRowCount = 8;
    static constexpr uint8_t kBitsPerRow = 5;
    static constexpr uint8_t kKeyDataMask = 0x1F;
    static constexpr uint8_t kFloatingBitsMask = 0xE0;

    enum class Key : uint8_t {
        Shift,
        Z,
        X,
        C,
        V,
        A,
        S,
        D,
        F,
        G,
        Q,
        W,
        E,
        R,
        T,
        Digit1,
        Digit2,
        Digit3,
        Digit4,
        Digit5,
        Digit0,
        Digit9,
        Digit8,
        Digit7,
        Digit6,
        P,
        O,
        I,
        U,
        Y,
        Enter,
        L,
        K,
        J,
        H,
        Space,
        Dot,
        M,
        N,
        B,
    };

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;

    static constexpr uint16_t rowPort(uint8_t row) {
        return static_cast<uint16_t>((0xFFu & static_cast<uint8_t>(~(1u << row))) << 8) | kPortLowByte;
    }

    static std::optional<uint8_t> rowFromPort(uint16_t port);
    static bool keyBit(Key key, uint8_t& row, uint8_t& bit);

    void setKeyDown(uint8_t row, uint8_t bit, bool down);
    bool keyDown(uint8_t row, uint8_t bit) const;
    void releaseAll();

    void pressKey(Key key);
    void releaseKey(Key key);

private:
    std::array<uint8_t, kRowCount> rows_{};
};

#endif
