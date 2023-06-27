#ifndef MOJAVE_CLOCK_HPP
#define MOJAVE_CLOCK_HPP

#include <chrono>
#include <cstdint>

class Clock {
public:
    static constexpr uint64_t kDefaultFrameHz = 60;

    explicit Clock(uint64_t hz = 0);

    void reset();
    void pace(uint64_t cyclesElapsed);
    bool unlimited() const { return hz_ == 0; }
    uint64_t hz() const { return hz_; }

    uint64_t cyclesPerFrame(uint64_t frameHz = kDefaultFrameHz) const;
    static uint32_t frameIntervalMs(uint64_t frameHz = kDefaultFrameHz);

private:
    uint64_t hz_;
    uint64_t cyclesDone_ = 0;
    std::chrono::steady_clock::time_point startTime_;
};

#endif
