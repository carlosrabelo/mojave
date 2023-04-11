#include "machine.hpp"

Machine::Machine(std::unique_ptr<Cpu> cpu)
    : cpu_(std::move(cpu)) {
    if (cpu_) {
        cpu_->setBus(&bus_);
    }
}

unsigned Machine::step() {
    unsigned cycles = cpu_->step();
    for (auto* dev : tick_list_)
        dev->tick(cycles);
    return cycles;
}

void Machine::reset() {
    cpu_->reset();
    for (auto& dev : owned_devices_)
        dev->reset();
}

void Machine::attachDevice(std::unique_ptr<Device> device,
                           uint16_t start, uint16_t end_exclusive) {
    Device* ptr = device.get();
    bus_.attach(*ptr, start, end_exclusive);
    tick_list_.push_back(ptr);
    owned_devices_.push_back(std::move(device));
}

void Machine::addOwnedDevice(std::unique_ptr<Device> device) {
    Device* ptr = device.get();
    tick_list_.push_back(ptr);
    owned_devices_.push_back(std::move(device));
}
