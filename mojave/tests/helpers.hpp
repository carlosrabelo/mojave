#ifndef MOJAVE_TEST_HELPERS_HPP
#define MOJAVE_TEST_HELPERS_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <utility>
#include "bus/bus.hpp"
#include "cpus/cpu.hpp"
#include "devices/device.hpp"
#include "devices/shared/memory.hpp"

struct BusWithRam {
    std::unique_ptr<Bus> bus;
    std::unique_ptr<Memory> ram;
};

inline std::unique_ptr<Memory> createMemory(uint16_t size) {
    return std::make_unique<Memory>(size);
}

inline std::unique_ptr<Memory> createMemoryFilled(uint16_t size, uint8_t pattern) {
    auto mem = std::make_unique<Memory>(size);
    for (uint16_t i = 0; i < size; ++i)
        mem->write(i, static_cast<uint8_t>(pattern + i));
    return mem;
}

inline BusWithRam createBusWithRam(uint16_t start, uint16_t end) {
    auto bus = std::make_unique<Bus>();
    auto ram = std::make_unique<Memory>(end - start);
    bus->attach(*ram, start, end);
    return {std::move(bus), std::move(ram)};
}

struct MockLogEntry {
    bool is_read;
    uint16_t address;
    uint8_t value;
};

class MockDevice : public Device {
public:
    std::vector<MockLogEntry> log;
    uint8_t read_value = 0;
    bool reset_called = false;
    unsigned total_tick_cycles = 0;
    unsigned tick_count = 0;

    uint8_t read(uint16_t address) override {
        log.push_back({true, address, read_value});
        return read_value;
    }

    void write(uint16_t address, uint8_t value) override {
        log.push_back({false, address, value});
    }

    void reset() override { reset_called = true; }

    void tick(unsigned cycles) override {
        total_tick_cycles += cycles;
        tick_count++;
    }
};

class MockCpu : public Cpu {
public:
    unsigned cycles_per_step = 4;
    unsigned step_count = 0;
    bool is_halted = false;
    bool reset_called = false;

    void reset() override {
        step_count = 0;
        is_halted = false;
        reset_called = true;
    }

    unsigned step() override {
        step_count++;
        return cycles_per_step;
    }

    bool halted() const override { return is_halted; }
};

#endif
