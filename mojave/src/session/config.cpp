#include "session/config.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {

std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
        s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.pop_back();
    return s;
}

bool isCommentLine(const std::string& line) {
    const std::string t = trim(line);
    return t.empty() || t[0] == '#' || t[0] == ';';
}

std::string parseSectionName(const std::string& line) {
    const std::size_t end = line.find(']');
    if (end == std::string::npos)
        return {};
    return trim(line.substr(1, end - 1));
}

bool splitKeyValue(const std::string& line, std::string& key, std::string& value) {
    const std::size_t eq = line.find('=');
    if (eq == std::string::npos)
        return false;
    key = trim(line.substr(0, eq));
    value = trim(line.substr(eq + 1));
    return !key.empty();
}


bool loadConfigFile(const std::string& path, MojaveConfig& out) {
    std::ifstream in(path);
    if (!in)
        return false;

    std::ostringstream ss;
    ss << in.rdbuf();
    out = mergeMojaveConfig(out, parseMojaveConfigText(ss.str()));
    return true;
}

std::string userConfigPath() {
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg != nullptr && xdg[0] != '\0')
        return std::string(xdg) + "/mojave/mojave.ini";

    if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0')
        return std::string(home) + "/.config/mojave/mojave.ini";

    return {};
}

} // namespace

std::string expandHomePath(std::string path) {
    if (path.empty() || path[0] != '~')
        return path;

    const char* home = std::getenv("HOME");
    if (home == nullptr || home[0] == '\0')
        return path;

    if (path.size() == 1 || path[1] == '/')
        return std::string(home) + path.substr(1);

    return path;
}

MojaveConfig parseMojaveConfigText(const std::string& text) {
    MojaveConfig config;
    std::string section;

    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        if (isCommentLine(line))
            continue;

        if (line.front() == '[') {
            section = parseSectionName(line);
            continue;
        }

        std::string key;
        std::string value;
        if (!splitKeyValue(line, key, value))
            continue;

        if (section == "roms")
            config.roms[key] = value;
    }

    return config;
}

MojaveConfig mergeMojaveConfig(const MojaveConfig& base, const MojaveConfig& overlay) {
    MojaveConfig merged = base;
    for (const auto& [key, value] : overlay.roms)
        merged.roms[key] = value;
    return merged;
}

MojaveConfig loadMojaveConfigFromPaths(const std::vector<std::string>& paths) {
    MojaveConfig config;
    for (const std::string& path : paths)
        loadConfigFile(path, config);
    return config;
}

MojaveConfig loadMojaveConfig() {
    std::vector<std::string> paths;
    const std::string user_path = userConfigPath();
    if (!user_path.empty())
        paths.push_back(user_path);
    paths.push_back("mojave.ini");
    return loadMojaveConfigFromPaths(paths);
}

std::string MojaveConfig::resolveLoadPath(const std::string& token) const {
    const auto it = roms.find(token);
    if (it == roms.end())
        return token;
    return expandHomePath(it->second);
}
