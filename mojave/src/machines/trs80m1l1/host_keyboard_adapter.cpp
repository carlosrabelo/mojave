#include "machines/trs80m1l1/host_keyboard_adapter.hpp"
#include "machines/trs80m1l1/typing_chord.hpp"

namespace {

constexpr bool isShiftBit(uint8_t row, uint8_t bit) {
    return row == 7 && bit == 0;
}

} // namespace

Trs80M1L1HostKeyboardAdapter::Trs80M1L1HostKeyboardAdapter(Trs80M1Keyboard& keyboard)
    : keyboard_(keyboard) {}

bool Trs80M1L1HostKeyboardAdapter::pulseIdle() const {
    return !pulse_active_ && pulse_gap_frames_left_ == 0 && pulse_queue_head_ == pulse_queue_tail_;
}

bool Trs80M1L1HostKeyboardAdapter::hasPendingInput() const {
    return !pulseIdle();
}

void Trs80M1L1HostKeyboardAdapter::tick() {
    tickPulse();
    for (uint8_t row = 0; row < Trs80M1Keyboard::kRowCount; ++row) {
        for (uint8_t bit = 0; bit < Trs80M1Keyboard::kBitsPerRow; ++bit)
            applyHardware(row, bit);
    }
    maybeReleaseShift();
}

void Trs80M1L1HostKeyboardAdapter::releaseAll() {
    host_down_.fill({});
    release_hold_.fill({});
    pending_shift_release_ = false;
    pulse_active_ = false;
    pulse_gap_frames_left_ = 0;
    pulse_queue_head_ = 0;
    pulse_queue_tail_ = 0;
    keyboard_.releaseAll();
}

bool Trs80M1L1HostKeyboardAdapter::enqueuePulse(const Pulse& pulse) {
    const uint8_t next_tail = static_cast<uint8_t>((pulse_queue_tail_ + 1) % pulse_queue_.size());
    if (next_tail == pulse_queue_head_)
        return false;
    pulse_queue_[pulse_queue_tail_] = pulse;
    pulse_queue_tail_ = next_tail;
    return true;
}

void Trs80M1L1HostKeyboardAdapter::startNextPulse() {
    if (pulse_queue_head_ == pulse_queue_tail_)
        return;
    const Pulse next = pulse_queue_[pulse_queue_head_];
    pulse_queue_head_ = static_cast<uint8_t>((pulse_queue_head_ + 1) % pulse_queue_.size());
    startPulse(next);
}

void Trs80M1L1HostKeyboardAdapter::pulseCharacter(char ch) {
    Trs80M1L1TypingChord chord;
    if (!trs80m1l1TypingChordForChar(ch, chord))
        return;

    Pulse pulse;
    pulse.row = chord.row;
    pulse.bit = chord.bit;
    pulse.shift = chord.shift;
    pulse.frames_left = kPulseFrames;

    if (pulseIdle()) {
        startPulse(pulse);
        return;
    }

    enqueuePulse(pulse);
}

void Trs80M1L1HostKeyboardAdapter::pulseSpecialKey(Trs80M1Keyboard::SpecialKey key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!Trs80M1Keyboard::specialKeyBit(key, row, bit))
        return;

    Pulse pulse;
    pulse.row = row;
    pulse.bit = bit;
    pulse.shift = false;
    pulse.frames_left = kPulseFrames;

    if (pulseIdle()) {
        startPulse(pulse);
        return;
    }

    enqueuePulse(pulse);
}

bool Trs80M1L1HostKeyboardAdapter::hostKeyDown(char name) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!Trs80M1Keyboard::namedKeyBit(name, row, bit))
        return false;
    setHostBit(row, bit, true);
    return true;
}

void Trs80M1L1HostKeyboardAdapter::hostKeyUp(char name) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!Trs80M1Keyboard::namedKeyBit(name, row, bit))
        return;
    setHostBit(row, bit, false);
}

bool Trs80M1L1HostKeyboardAdapter::hostSpecialDown(Trs80M1Keyboard::SpecialKey key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!Trs80M1Keyboard::specialKeyBit(key, row, bit))
        return false;
    setHostBit(row, bit, true);
    return true;
}

void Trs80M1L1HostKeyboardAdapter::hostSpecialUp(Trs80M1Keyboard::SpecialKey key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!Trs80M1Keyboard::specialKeyBit(key, row, bit))
        return;
    setHostBit(row, bit, false);
}

void Trs80M1L1HostKeyboardAdapter::syncHostShift(bool shift_down) {
    if (shift_down) {
        pending_shift_release_ = false;
        setHostBit(7, 0, true);
        return;
    }

    if (nonShiftKeysActive()) {
        pending_shift_release_ = true;
        return;
    }

    setHostBit(7, 0, false);
}

void Trs80M1L1HostKeyboardAdapter::setHostBit(uint8_t row, uint8_t bit, bool down) {
    if (row >= Trs80M1Keyboard::kRowCount || bit >= Trs80M1Keyboard::kBitsPerRow)
        return;

    host_down_[row][bit] = down;
    if (down) {
        release_hold_[row][bit] = 0;
        pressHardware(row, bit);
        return;
    }

    release_hold_[row][bit] = kReleaseHoldFrames;
}

void Trs80M1L1HostKeyboardAdapter::pressHardware(uint8_t row, uint8_t bit) {
    keyboard_.setKeyDown(row, bit, true);
}

void Trs80M1L1HostKeyboardAdapter::releaseHardware(uint8_t row, uint8_t bit) {
    keyboard_.setKeyDown(row, bit, false);
}

void Trs80M1L1HostKeyboardAdapter::applyHardware(uint8_t row, uint8_t bit) {
    if (host_down_[row][bit]) {
        if (!keyboard_.keyDown(row, bit))
            pressHardware(row, bit);
        return;
    }

    uint8_t& hold = release_hold_[row][bit];
    if (hold == 0)
        return;

    --hold;
    if (hold == 0)
        releaseHardware(row, bit);
}

bool Trs80M1L1HostKeyboardAdapter::nonShiftKeysActive() const {
    for (uint8_t row = 0; row < Trs80M1Keyboard::kRowCount; ++row) {
        for (uint8_t bit = 0; bit < Trs80M1Keyboard::kBitsPerRow; ++bit) {
            if (isShiftBit(row, bit))
                continue;
            if (host_down_[row][bit] || release_hold_[row][bit] > 0)
                return true;
        }
    }
    return pulse_active_;
}

void Trs80M1L1HostKeyboardAdapter::maybeReleaseShift() {
    if (!pending_shift_release_ || nonShiftKeysActive())
        return;
    pending_shift_release_ = false;
    host_down_[7][0] = false;
    release_hold_[7][0] = 0;
    releaseHardware(7, 0);
}

void Trs80M1L1HostKeyboardAdapter::startPulse(const Pulse& pulse) {
    active_pulse_ = pulse;
    pulse_active_ = true;
    pressHardware(pulse.row, pulse.bit);
    if (pulse.shift)
        pressHardware(7, 0);
}

void Trs80M1L1HostKeyboardAdapter::tickPulse() {
    if (pulse_gap_frames_left_ > 0) {
        --pulse_gap_frames_left_;
        if (pulse_gap_frames_left_ == 0)
            startNextPulse();
        return;
    }

    if (!pulse_active_)
        return;

    if (active_pulse_.frames_left > 0)
        --active_pulse_.frames_left;

    if (active_pulse_.frames_left > 0)
        return;

    releaseHardware(active_pulse_.row, active_pulse_.bit);
    if (active_pulse_.shift)
        releaseHardware(7, 0);
    pulse_active_ = false;

    if (pulse_queue_head_ == pulse_queue_tail_)
        return;

    pulse_gap_frames_left_ = kPulseGapFrames;
}
