#include <cstdio>
#include <string>
#include "session/cli.hpp"
#include "session/config.hpp"

int main(int argc, char* argv[]) {
    const MojaveConfig config = loadMojaveConfig();
    CLIOptions opts = parseCLI(argc, argv, config);
    if (!opts.ok) {
        std::fprintf(stderr, "Error: %s\n", opts.error.c_str());
        return 1;
    }
    if (opts.help) {
        return 0;
    }

    std::fprintf(stderr, "Error: machine preset '%s' not supported or implemented yet.\n",
                 opts.machine.c_str());
    return 1;
}
