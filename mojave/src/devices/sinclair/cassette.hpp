#ifndef MOJAVE_SINCLAIR_CASSETTE_HPP
#define MOJAVE_SINCLAIR_CASSETTE_HPP

#include <cstdint>

// Sinclair ZX-80/ZX-81 cassette EAR/MIC stub.
//
// Hardware ties MIC high on any OUT and low on IN from port 0xFE (A0 low).
// EAR is sampled on bit 7 of the same reads. Full ~250 baud waveform
// decode/encode is deferred; this stub exposes idle levels the ROM expects.
class SinclairCassette {
public:
    static constexpr uint32_t kBaud = 250;
    static constexpr uint8_t kEarBit = 0x80;
    static constexpr uint8_t kEarIdleMask = 0x80;

    void reset();

    uint8_t earBits() const;
    bool micHigh() const { return mic_high_; }

    void onFeRead();
    void onAnyOut();

    void setEarHigh(bool high);

private:
    bool mic_high_ = true;
    bool ear_high_ = true;
};

#endif
