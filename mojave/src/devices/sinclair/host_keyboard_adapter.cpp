#include "devices/sinclair/host_keyboard_adapter.hpp"

SinclairHostKeyboardAdapter::SinclairHostKeyboardAdapter(SinclairKeyboard& keyboard) : keyboard_(keyboard) {}

bool SinclairHostKeyboardAdapter::pulseIdle() const {
    return !pulse_active_ && pulse_gap_frames_left_ == 0 && pulse_queue_head_ == pulse_queue_tail_;
}

bool SinclairHostKeyboardAdapter::hasPendingInput() const {
    return !pulseIdle();
}

void SinclairHostKeyboardAdapter::tick() {
    tickPulse();
    for (uint8_t row = 0; row < SinclairKeyboard::kRowCount; ++row) {
        for (uint8_t bit = 0; bit < SinclairKeyboard::kBitsPerRow; ++bit)
            applyHardware(row, bit);
    }
}

void SinclairHostKeyboardAdapter::releaseAll() {
    host_down_.fill({});
    release_hold_.fill({});
    pulse_active_ = false;
    pulse_gap_frames_left_ = 0;
    pulse_queue_head_ = 0;
    pulse_queue_tail_ = 0;
    keyboard_.releaseAll();
}

bool SinclairHostKeyboardAdapter::hostKeyDown(SinclairKeyboard::Key key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!SinclairKeyboard::keyBit(key, row, bit))
        return false;
    setHostBit(row, bit, true);
    return true;
}

void SinclairHostKeyboardAdapter::hostKeyUp(SinclairKeyboard::Key key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!SinclairKeyboard::keyBit(key, row, bit))
        return;
    setHostBit(row, bit, false);
}

bool SinclairHostKeyboardAdapter::enqueuePulse(const Pulse& pulse) {
    const uint8_t next_tail = static_cast<uint8_t>((pulse_queue_tail_ + 1) % pulse_queue_.size());
    if (next_tail == pulse_queue_head_)
        return false;
    pulse_queue_[pulse_queue_tail_] = pulse;
    pulse_queue_tail_ = next_tail;
    return true;
}

void SinclairHostKeyboardAdapter::startNextPulse() {
    if (pulse_queue_head_ == pulse_queue_tail_)
        return;
    const Pulse next = pulse_queue_[pulse_queue_head_];
    pulse_queue_head_ = static_cast<uint8_t>((pulse_queue_head_ + 1) % pulse_queue_.size());
    startPulse(next);
}

void SinclairHostKeyboardAdapter::pulseCharacter(const SinclairTypingChord& chord) {
    Pulse pulse;
    pulse.key = chord.key;
    pulse.shift = chord.shift;
    pulse.frames_left = kPulseFrames;

    if (pulseIdle()) {
        startPulse(pulse);
        return;
    }

    enqueuePulse(pulse);
}

void SinclairHostKeyboardAdapter::pulseKey(SinclairKeyboard::Key key, bool shift) {
    SinclairTypingChord chord;
    chord.key = key;
    chord.shift = shift;
    pulseCharacter(chord);
}

void SinclairHostKeyboardAdapter::setHostBit(uint8_t row, uint8_t bit, bool down) {
    if (row >= SinclairKeyboard::kRowCount || bit >= SinclairKeyboard::kBitsPerRow)
        return;

    host_down_[row][bit] = down;
    if (down) {
        release_hold_[row][bit] = 0;
        pressHardware(row, bit);
        return;
    }

    // A short release hold keeps the matrix bit asserted long enough for the
    // Sinclair ROM keyboard scan to read the key twice in succession (its debounce
    // requirement). Without it, a quick tap whose host key-down/key-up arrive in
    // the same event poll is never seen by the guest and the key is lost. The
    // hold is kept short so that normal typing does not overlap keys (a long hold
    // makes the ROM's bitmask scan see two keys at once and emit garbage).
    release_hold_[row][bit] = kReleaseHoldFrames;
}

void SinclairHostKeyboardAdapter::pressHardware(uint8_t row, uint8_t bit) {
    keyboard_.setKeyDown(row, bit, true);
}

void SinclairHostKeyboardAdapter::releaseHardware(uint8_t row, uint8_t bit) {
    keyboard_.setKeyDown(row, bit, false);
}

void SinclairHostKeyboardAdapter::pressKey(SinclairKeyboard::Key key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!SinclairKeyboard::keyBit(key, row, bit))
        return;
    pressHardware(row, bit);
}

void SinclairHostKeyboardAdapter::releaseKey(SinclairKeyboard::Key key) {
    uint8_t row = 0;
    uint8_t bit = 0;
    if (!SinclairKeyboard::keyBit(key, row, bit))
        return;
    releaseHardware(row, bit);
}

void SinclairHostKeyboardAdapter::applyHardware(uint8_t row, uint8_t bit) {
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

void SinclairHostKeyboardAdapter::startPulse(const Pulse& pulse) {
    active_pulse_ = pulse;
    pulse_active_ = true;
    if (pulse.shift)
        pressKey(SinclairKeyboard::Key::Shift);
    pressKey(pulse.key);
}

void SinclairHostKeyboardAdapter::tickPulse() {
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

    releaseKey(active_pulse_.key);
    if (active_pulse_.shift)
        releaseKey(SinclairKeyboard::Key::Shift);
    pulse_active_ = false;

    if (pulse_queue_head_ == pulse_queue_tail_)
        return;

    pulse_gap_frames_left_ = kPulseGapFrames;
}
