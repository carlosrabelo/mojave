#ifndef MOJAVE_SINCLAIR_HOST_KEYBOARD_ADAPTER_HPP
#define MOJAVE_SINCLAIR_HOST_KEYBOARD_ADAPTER_HPP

#include <array>
#include <cstdint>
#include "devices/sinclair/keyboard.hpp"
#include "devices/sinclair/typing_chord.hpp"

class SinclairHostKeyboardAdapter {
public:
    static constexpr uint8_t kReleaseHoldFrames = 2;
    static constexpr uint8_t kPulseFrames = 4;
    static constexpr uint8_t kPulseGapFrames = 2;
    static constexpr uint8_t kPulseQueueCapacity = 128;

    explicit SinclairHostKeyboardAdapter(SinclairKeyboard& keyboard);

    void tick();
    void releaseAll();
    bool hasPendingInput() const;

    bool hostKeyDown(SinclairKeyboard::Key key);
    void hostKeyUp(SinclairKeyboard::Key key);
    void pulseCharacter(const SinclairTypingChord& chord);
    void pulseKey(SinclairKeyboard::Key key, bool shift);

private:
    struct Pulse {
        SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;
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
    void pressKey(SinclairKeyboard::Key key);
    void releaseKey(SinclairKeyboard::Key key);
    void startPulse(const Pulse& pulse);
    void tickPulse();

    SinclairKeyboard& keyboard_;
    std::array<std::array<bool, SinclairKeyboard::kBitsPerRow>, SinclairKeyboard::kRowCount> host_down_{};
    std::array<std::array<uint8_t, SinclairKeyboard::kBitsPerRow>, SinclairKeyboard::kRowCount> release_hold_{};
    bool pulse_active_ = false;
    uint8_t pulse_gap_frames_left_ = 0;
    Pulse active_pulse_{};
    std::array<Pulse, kPulseQueueCapacity> pulse_queue_{};
    uint8_t pulse_queue_head_ = 0;
    uint8_t pulse_queue_tail_ = 0;
};

#endif
