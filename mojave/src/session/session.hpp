#ifndef MOJAVE_SESSION_HPP
#define MOJAVE_SESSION_HPP

#include <cstdint>
#include "machines/shared/machine.hpp"
#include "devices/shared/virtual_tty.hpp"

class Session {
public:
    Session(Machine& machine, VirtualTTY& tty);

    void runUntilHalt();
    void reset();

    unsigned stepCount() const { return steps_; }
    uint64_t totalCycles() const { return cycles_; }

private:
    void pollTTY();

    Machine& machine_;
    VirtualTTY& tty_;
    unsigned steps_ = 0;
    uint64_t cycles_ = 0;
};

#endif
