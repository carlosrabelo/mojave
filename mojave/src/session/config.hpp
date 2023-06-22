#ifndef MOJAVE_CONFIG_HPP
#define MOJAVE_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct MojaveConfig {
    std::unordered_map<std::string, std::string> roms;

    // If token is a [roms] alias, return the configured path (with ~ expanded).
    // Otherwise return token unchanged.
    std::string resolveLoadPath(const std::string& token) const;
};

// Parse INI text; later sections/keys override earlier ones in the same parse call.
MojaveConfig parseMojaveConfigText(const std::string& text);

// Merge configs; keys from overlay replace keys from base.
MojaveConfig mergeMojaveConfig(const MojaveConfig& base, const MojaveConfig& overlay);

// Load mojave.ini from ~/.config/mojave/ (or $XDG_CONFIG_HOME) then ./mojave.ini (cwd wins).
MojaveConfig loadMojaveConfig();

// Load and merge the given paths in order (later files override earlier keys).
MojaveConfig loadMojaveConfigFromPaths(const std::vector<std::string>& paths);

// Expand a leading ~ or ~/ in path using $HOME.
std::string expandHomePath(std::string path);

#endif
