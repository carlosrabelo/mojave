#include <cstdlib>
#include <cstring>
#include "catch.hpp"
#include "session/cli.hpp"
#include "session/config.hpp"

TEST_CASE("CLI parses --help", "[cli][fast]") {
    const char* argv[] = {"mojave", "--help"};
    auto opts = parseCLI(2, const_cast<char**>(argv));
    REQUIRE(opts.help);
    REQUIRE(opts.ok);
}

TEST_CASE("CLI parses -h short form", "[cli][fast]") {
    const char* argv[] = {"mojave", "-h"};
    auto opts = parseCLI(2, const_cast<char**>(argv));
    REQUIRE(opts.help);
}

TEST_CASE("CLI defaults to z80 machine and shows help on empty args", "[cli][fast]") {
    const char* argv[] = {"mojave"};
    auto opts = parseCLI(1, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.help);
    REQUIRE(opts.machine == "z80");
}

TEST_CASE("CLI rejects --machine before any preset exists", "[cli][fast]") {
    const char* argv[] = {"mojave", "--machine", "m6502"};
    auto opts = parseCLI(3, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
    REQUIRE(opts.error.find("unknown machine") != std::string::npos);
}

TEST_CASE("CLI rejects unknown machine", "[cli][fast]") {
    const char* argv[] = {"mojave", "--machine", "x86"};
    auto opts = parseCLI(3, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
    REQUIRE(opts.error.find("unknown machine") != std::string::npos);
}

TEST_CASE("CLI rejects --machine without value", "[cli][fast]") {
    const char* argv[] = {"mojave", "--machine"};
    auto opts = parseCLI(2, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
    REQUIRE(opts.error.find("requires") != std::string::npos);
}

TEST_CASE("CLI parses --load-bin with hex address", "[cli][fast]") {
    const char* argv[] = {"mojave", "--load-bin", "rom.bin", "8000"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.loads.size() == 1);
    REQUIRE(opts.loads[0].path == "rom.bin");
    REQUIRE(opts.loads[0].address == 0x8000);
}

TEST_CASE("CLI rejects --load-bin with bad hex", "[cli][fast]") {
    const char* argv[] = {"mojave", "--load-bin", "rom.bin", "xyz"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
    REQUIRE(opts.error.find("invalid address or alias") != std::string::npos);
}

TEST_CASE("CLI parses --dump-mem range", "[cli][fast]") {
    const char* argv[] = {"mojave", "--dump-mem", "0000", "00FF"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.dumpMem);
    REQUIRE(opts.memStart == 0x0000);
    REQUIRE(opts.memEnd == 0x00FF);
}

TEST_CASE("CLI rejects --dump-mem with end <= start", "[cli][fast]") {
    const char* argv[] = {"mojave", "--dump-mem", "00FF", "0000"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
}

TEST_CASE("CLI parses --dump-reg", "[cli][fast]") {
    const char* argv[] = {"mojave", "--dump-reg"};
    auto opts = parseCLI(2, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.dumpReg);
}

TEST_CASE("CLI rejects unknown option", "[cli][fast]") {
    const char* argv[] = {"mojave", "--bogus"};
    auto opts = parseCLI(2, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
    REQUIRE(opts.error.find("unknown option") != std::string::npos);
}

TEST_CASE("CLI combined options parse correctly", "[cli][fast]") {
    const char* argv[] = {
        "mojave",
        "--load-bin", "code.bin", "0600",
        "--dump-reg",
        "--dump-mem", "0600", "0610"
    };
    auto opts = parseCLI(8, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.machine == "z80");
    REQUIRE(opts.loads.size() == 1);
    REQUIRE(opts.loads[0].path == "code.bin");
    REQUIRE(opts.loads[0].address == 0x0600);
    REQUIRE(opts.dumpReg);
    REQUIRE(opts.dumpMem);
    REQUIRE(opts.memStart == 0x0600);
    REQUIRE(opts.memEnd == 0x0610);
}

TEST_CASE("CLI accepts --load-bin without address argument, defaulting to rom/boot address", "[cli][fast]") {
    const char* argv[] = {"mojave", "--load-bin", "rom.bin"};
    auto opts = parseCLI(3, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.loads.size() == 1);
    REQUIRE(opts.loads[0].path == "rom.bin");
    REQUIRE(opts.loads[0].address == 0x0000);
}


TEST_CASE("CLI resolves rom alias for z80", "[cli][fast]") {
    const char* argv[] = {"mojave", "--load-bin", "code.bin", "rom"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.loads[0].address == 0x0000);
}

TEST_CASE("CLI resolves ram alias for z80", "[cli][fast]") {
    const char* argv[] = {"mojave", "--load-bin", "code.bin", "ram"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE(opts.ok);
    REQUIRE(opts.loads[0].address == 0x8000);
}




TEST_CASE("CLI rejects unknown alias", "[cli][fast]") {
    const char* argv[] = {"mojave", "--load-bin", "code.bin", "bank2"};
    auto opts = parseCLI(4, const_cast<char**>(argv));
    REQUIRE_FALSE(opts.ok);
    REQUIRE(opts.error.find("invalid address or alias") != std::string::npos);
}


