#ifndef MOJAVE_CPU_HPP
#define MOJAVE_CPU_HPP

#include <cstdint>
#include <string>
#include <vector>

class Bus;

struct RegisterEntry {
    std::string name;
    uint16_t value;
};

struct RegisterSnapshot {
    std::vector<RegisterEntry> entries;
};

class Cpu {
public:
    virtual ~Cpu() = default;

    virtual void reset() = 0;
    virtual unsigned step() = 0;
    virtual bool halted() const = 0;

    virtual RegisterSnapshot registers() const {
        return {};
    }

    virtual void setBus(Bus* bus) {
        bus_ = bus;
        updatePageTable();
    }

    virtual void updatePageTable() {}

protected:
    Bus* bus_ = nullptr;
};


#endif
