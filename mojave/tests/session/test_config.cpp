#include <cstdlib>
#include <fstream>
#include "catch.hpp"
#include "session/config.hpp"

TEST_CASE("mojave.ini [roms] section parses alias paths", "[config][fast]") {
    const std::string text =
        "# user roms\n"
        "[roms]\n"
        "trs80m1l1=~/Sources/roms/mode1level1.bin\n"
        "trs80m3=~/Sources/roms/model3.bin\n";

    const MojaveConfig config = parseMojaveConfigText(text);
    REQUIRE(config.roms.size() == 2);
    REQUIRE(config.roms.at("trs80m1l1") == "~/Sources/roms/mode1level1.bin");
    REQUIRE(config.roms.at("trs80m3") == "~/Sources/roms/model3.bin");
}

TEST_CASE("expandHomePath expands leading tilde", "[config][fast]") {
#if defined(_WIN32)
    _putenv_s("HOME", "/home/tester");
#else
    setenv("HOME", "/home/tester", 1);
#endif
    REQUIRE(expandHomePath("~/roms/a.bin") == "/home/tester/roms/a.bin");
    REQUIRE(expandHomePath("/abs/path.bin") == "/abs/path.bin");
}

TEST_CASE("resolveLoadPath maps rom alias and expands home", "[config][fast]") {
#if defined(_WIN32)
    _putenv_s("HOME", "/home/tester");
#else
    setenv("HOME", "/home/tester", 1);
#endif

    MojaveConfig config;
    config.roms["trs80m1l1"] = "~/Sources/roms/mode1level1.bin";

    REQUIRE(config.resolveLoadPath("trs80m1l1") == "/home/tester/Sources/roms/mode1level1.bin");
    REQUIRE(config.resolveLoadPath("plain/path.bin") == "plain/path.bin");
}

TEST_CASE("mergeMojaveConfig overlays rom aliases", "[config][fast]") {
    MojaveConfig base;
    base.roms["trs80m1l1"] = "/global/old.bin";

    MojaveConfig overlay;
    overlay.roms["trs80m1l1"] = "/local/new.bin";
    overlay.roms["trs80m3"] = "/local/model3.bin";

    const MojaveConfig merged = mergeMojaveConfig(base, overlay);
    REQUIRE(merged.roms.at("trs80m1l1") == "/local/new.bin");
    REQUIRE(merged.roms.at("trs80m3") == "/local/model3.bin");
}

TEST_CASE("loadMojaveConfigFromPaths merges later files over earlier ones", "[config][fast]") {
    const std::string user_dir = "build/test-config-user/mojave";
    const std::string user_file = user_dir + "/mojave.ini";
    const std::string local_file = "build/test-config-local/mojave.ini";

    std::system(("mkdir -p " + user_dir + " build/test-config-local").c_str());

    {
        std::ofstream out(user_file);
        out << "[roms]\n"
            << "trs80m1l1=/global/mode1.bin\n"
            << "trs80m3=/global/model3.bin\n";
    }
    {
        std::ofstream out(local_file);
        out << "[roms]\n"
            << "trs80m1l1=/local/mode1.bin\n";
    }

    const MojaveConfig config = loadMojaveConfigFromPaths({user_file, local_file});
    REQUIRE(config.roms.at("trs80m1l1") == "/local/mode1.bin");
    REQUIRE(config.roms.at("trs80m3") == "/global/model3.bin");
}
