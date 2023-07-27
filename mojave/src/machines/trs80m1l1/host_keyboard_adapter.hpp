#ifndef MOJAVE_TRS80M1L1_HOST_KEYBOARD_ADAPTER_HPP
#define MOJAVE_TRS80M1L1_HOST_KEYBOARD_ADAPTER_HPP

#include <array>
#include <cstdint>
#include "devices/trs80m1/keyboard.hpp"

class Trs80M1L1HostKeyboardAdapter {
public:
    static constexpr uint8_t kReleaseHoldFrames = 8;
    static constexpr uint8_t kPulseFrames = 4;
    static constexpr uint8_t kPulseGapFrames = 2;
    static constexpr uint8_t kPulseQueueCapacity = 128;

    explicit Trs80M1L1HostKeyboardAdapter(Trs80M1Keyboard& keyboard);

    void tick();
    void releaseAll();
    bool hasPendingInput() const;

    void pulseCharacter(char ch);
    void pulseSpecialKey(Trs80M1Keyboard::SpecialKey key);
    bool hostKeyDown(char name);
    void hostKeyUp(char name);
    bool hostSpecialDown(Trs80M1Keyboard::SpecialKey key);
    void hostSpecialUp(Trs80M1Keyboard::SpecialKey key);
    void syncHostShift(bool shift_down);

private:
    struct Pulse {
        uint8_t row = 0;
        uint8_t bit = 0;
        bool shift = false;
        uint8_t frames_left = 0;
    };

    bool pulseIdle() const;
    bool enqueuePulse(const Pulse& pulse);
    void startNextPulse();
    void setHostBit(uint8_t row, uint8_t bit, bool down);
    void applyHardware(uint8_t row, uint8_t bit);
    void pressHardware(uint8_t row, uint8_t bit);
    void releaseHardware(uint8_t row, uint8_t bit);
    bool nonShiftKeysActive() const;
    void maybeReleaseShift();
    void startPulse(const Pulse& pulse);
    void tickPulse();

    Trs80M1Keyboard& keyboard_;
    std::array<std::array<bool, Trs80M1Keyboard::kBitsPerRow>, Trs80M1Keyboard::kRowCount> host_down_{};
    std::array<std::array<uint8_t, Trs80M1Keyboard::kBitsPerRow>, Trs80M1Keyboard::kRowCount> release_hold_{};
    bool pending_shift_release_ = false;
    bool pulse_active_ = false;
    uint8_t pulse_gap_frames_left_ = 0;
    Pulse active_pulse_{};
    std::array<Pulse, kPulseQueueCapacity> pulse_queue_{};
    uint8_t pulse_queue_head_ = 0;
    uint8_t pulse_queue_tail_ = 0;
};

#endif
