#include "machines/zx80/keyboard_scan.hpp"
#include "devices/sinclair/keyboard.hpp"

uint16_t zx80SimulateRomKeyboardScan(SinclairKeyboard& keyboard) {
    uint8_t H = 0xFF;
    uint8_t L = 0xFF;
    uint8_t B = 0xFE;

    uint16_t port = static_cast<uint16_t>(B) << 8 | SinclairKeyboard::kPortLowByte;
    uint8_t A = keyboard.readPort(port);
    A = static_cast<uint8_t>(A | 0x01);

    for (;;) {
        A = static_cast<uint8_t>(A | 0xE0);
        const uint8_t D = A;
        A = static_cast<uint8_t>(~A);
        const bool carry = A < 1;
        A = carry ? 0xFF : 0;
        A = static_cast<uint8_t>(A | B);
        L = static_cast<uint8_t>(L & A);
        H = static_cast<uint8_t>(H & D);

        const bool old_bit7 = (B & 0x80) != 0;
        B = static_cast<uint8_t>((B << 1) | (old_bit7 ? 1 : 0));

        port = static_cast<uint16_t>(B) << 8 | SinclairKeyboard::kPortLowByte;
        A = keyboard.readPort(port);

        if (!old_bit7)
            break;
    }

    A = keyboard.readPort(static_cast<uint16_t>(0xFEu << 8) | SinclairKeyboard::kPortLowByte);
    const bool shift_bit = (A & 0x01) != 0;
    H = static_cast<uint8_t>(((H << 1) & 0xFF) | (shift_bit ? 1 : 0));

    return static_cast<uint16_t>((static_cast<uint16_t>(H) << 8) | L);
}
