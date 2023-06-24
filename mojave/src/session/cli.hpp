#ifndef MOJAVE_CLI_HPP
#define MOJAVE_CLI_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "session/config.hpp"

struct LoadEntry {
    std::string path;
    uint16_t address;
};

struct CLIOptions {
    std::string machine = "z80";
    std::vector<LoadEntry> loads;
    bool dumpReg = false;
    bool dumpMem = false;
    uint16_t memStart = 0;
    uint16_t memEnd = 0;
    bool help = false;

    bool ok = true;
    std::string error;
};

CLIOptions parseCLI(int argc, char* argv[], const MojaveConfig& config = {});

#endif
