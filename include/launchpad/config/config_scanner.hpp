#pragma once

#include "launchpad/config/ini_parser.hpp"
#include "launchpad/config/settings.hpp"
#include <string>
#include <vector>

namespace launchpad::config {

struct ScannedConfig {
    LaunchpadSettings settings;
    std::vector<IniEntry> actions;
};

class ConfigScanner {
public:
    static ScannedConfig scan_directory(const std::string& directory_path);

private:
    static std::vector<std::string> find_ini_files(const std::string& directory_path);
    static void apply_settings(LaunchpadSettings& settings, const IniDocument& doc);
};

} // namespace launchpad::config
