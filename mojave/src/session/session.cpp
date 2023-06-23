#include "session/session.hpp"
#include <cstdio>
#include <sys/select.h>
#include <unistd.h>

Session::Session(Machine& machine, VirtualTTY& tty)
    : machine_(machine), tty_(tty) {}

void Session::reset() {
    steps_ = 0;
    cycles_ = 0;
}

void Session::runUntilHalt() {
    while (!machine_.cpu().halted()) {
        unsigned cycles = machine_.step();
        steps_++;
        cycles_ += cycles;
        pollTTY();
    }
}

void Session::pollTTY() {
    // Drain TTY output to stdout
    while (true) {
        int c = tty_.readChar();
        if (c < 0) break;
        std::putchar(static_cast<char>(c));
    }
    if (steps_ > 0)
        std::fflush(stdout);

    // Non-blocking check for stdin input
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv = {0, 0};

    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0) {
        char buf[64];
        ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
        for (ssize_t i = 0; i < n; ++i)
            tty_.injectChar(static_cast<uint8_t>(buf[i]));
    }
}
