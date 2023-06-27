#include "session/clock.hpp"
#include <thread>

Clock::Clock(uint64_t hz)
    : hz_(hz) {
    reset();
}

void Clock::reset() {
    cyclesDone_ = 0;
    startTime_ = std::chrono::steady_clock::now();
}

void Clock::pace(uint64_t cyclesElapsed) {
    cyclesDone_ += cyclesElapsed;

    if (hz_ == 0) return;

    // Expected elapsed time in microseconds
    uint64_t expectedUs = (cyclesDone_ * 1'000'000) / hz_;
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - startTime_);

    if (elapsed.count() < static_cast<int64_t>(expectedUs)) {
        auto sleepUs = expectedUs - static_cast<uint64_t>(elapsed.count());
        std::this_thread::sleep_for(std::chrono::microseconds(sleepUs));
    }
}

uint64_t Clock::cyclesPerFrame(uint64_t frameHz) const {
    if (hz_ == 0 || frameHz == 0)
        return 0;
    return hz_ / frameHz;
}

uint32_t Clock::frameIntervalMs(uint64_t frameHz) {
    if (frameHz == 0)
        return 0;
    return static_cast<uint32_t>(1000 / frameHz);
}
