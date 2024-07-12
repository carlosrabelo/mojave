#include "devices/sinclair/cassette.hpp"

void SinclairCassette::reset() {
    mic_high_ = true;
    ear_high_ = true;
}

uint8_t SinclairCassette::earBits() const {
    return ear_high_ ? kEarIdleMask : 0;
}

void SinclairCassette::onFeRead() {
    mic_high_ = false;
}

void SinclairCassette::onAnyOut() {
    mic_high_ = true;
}

void SinclairCassette::setEarHigh(bool high) {
    ear_high_ = high;
}
