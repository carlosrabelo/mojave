#include "session/cli.hpp"
#include "session/config.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void usage() {
    std::printf(
        "Usage: mojave [options]\n"
        "\n"
        "Options:\n"
        "  --machine <name>       Machine preset (z80)\n"
        "  --load-bin <path> [<addr>]\n"
        "                         Load binary at hex address/alias; path may be a [roms]\n"
        "                         alias from mojave.ini (defaults to boot/rom)\n"
        "  --dump-mem <start> <end>\n"
        "                         Dump memory range as hex after run\n"
        "  --dump-reg             Print CPU registers after run\n"
        "  -h, --help             Show this help message\n"
    );
}

static bool parseHex(const char* s, uint16_t& out) {
    char* end = nullptr;
    unsigned long v = std::strtoul(s, &end, 16);
    if (end == s || *end != '\0' || v > 0xFFFF) return false;
    out = static_cast<uint16_t>(v);
    return true;
}

static bool isKnownMachine(const std::string& machine) {
    return machine == "z80";
}

static bool resolveAddr(const char* s, const std::string& machine, uint16_t& out) {
    if (std::strcmp(s, "rom") == 0) {
        if (machine == "z80")   { out = 0x0000; return true; }
        if (machine == "m6502") { out = 0x0000; return true; }
        if (machine == "m6507") { out = 0x1000; return true; }
    }
    if (std::strcmp(s, "ram") == 0) {
        if (machine == "z80")   { out = 0x8000; return true; }
        if (machine == "m6502") { out = 0x0000; return true; }
        if (machine == "m6507") { out = 0x0000; return true; }
    }
    return parseHex(s, out);
}

CLIOptions parseCLI(int argc, char* argv[], const MojaveConfig& config) {
    CLIOptions opts;

    if (argc < 2) {
        usage();
        opts.help = true;
        return opts;
    }

    // First pass: extract --machine so aliases resolve correctly
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--machine") == 0 && i + 1 < argc) {
            opts.machine = argv[i + 1];
        }
    }

    // Validate machine
    if (!isKnownMachine(opts.machine)) {
        // Check if --machine was explicitly given
        for (int i = 1; i < argc; ++i) {
            if (std::strcmp(argv[i], "--machine") == 0) {
                opts.ok = false;
                opts.error = "unknown machine: " + opts.machine;
                return opts;
            }
        }
    }

    // Second pass: process all options
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0) {
            usage();
            opts.help = true;
            return opts;
        }
        else if (std::strcmp(argv[i], "--machine") == 0) {
            if (i + 1 >= argc) {
                opts.ok = false;
                opts.error = "--machine requires a machine name";
                return opts;
            }
            ++i; // skip value (already captured in first pass)
        }
        else if (std::strcmp(argv[i], "--load-bin") == 0) {
            if (i + 1 >= argc) {
                opts.ok = false;
                opts.error = "--load-bin requires a path argument";
                return opts;
            }
            std::string path = config.resolveLoadPath(argv[++i]);
            const char* addr_str = "rom";
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                addr_str = argv[++i];
            }
            uint16_t addr;
            if (!resolveAddr(addr_str, opts.machine, addr)) {
                opts.ok = false;
                opts.error = "--load-bin: invalid address or alias: " + std::string(addr_str);
                return opts;
            }
            opts.loads.push_back({path, addr});
        }
        else if (std::strcmp(argv[i], "--dump-mem") == 0) {
            if (i + 2 >= argc) {
                opts.ok = false;
                opts.error = "--dump-mem requires two hex addresses (start end)";
                return opts;
            }
            if (!parseHex(argv[++i], opts.memStart) || !parseHex(argv[++i], opts.memEnd)) {
                opts.ok = false;
                opts.error = "--dump-mem: invalid hex address range";
                return opts;
            }
            if (opts.memEnd <= opts.memStart) {
                opts.ok = false;
                opts.error = "--dump-mem: end must be greater than start";
                return opts;
            }
            opts.dumpMem = true;
        }
        else if (std::strcmp(argv[i], "--dump-reg") == 0) {
            opts.dumpReg = true;
        }
        else {
            opts.ok = false;
            opts.error = "unknown option: " + std::string(argv[i]);
            return opts;
        }
    }

    return opts;
}
