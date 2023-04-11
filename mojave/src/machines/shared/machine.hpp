#ifndef MOJAVE_MACHINE_HPP
#define MOJAVE_MACHINE_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "bus/bus.hpp"
#include "cpus/cpu.hpp"
#include "devices/device.hpp"

class Machine {
public:
    Machine(std::unique_ptr<Cpu> cpu);

    unsigned step();
    void reset();

    void attachDevice(std::unique_ptr<Device> device,
                      uint16_t start, uint16_t end_exclusive);

    void addOwnedDevice(std::unique_ptr<Device> device);

    Bus& bus() { return bus_; }
    Cpu& cpu() { return *cpu_; }
    const std::vector<std::unique_ptr<Device>>& ownedDevices() const { return owned_devices_; }

private:
    std::unique_ptr<Cpu> cpu_;
    Bus bus_;
    std::vector<std::unique_ptr<Device>> owned_devices_;
    std::vector<Device*> tick_list_;
};

#endif
