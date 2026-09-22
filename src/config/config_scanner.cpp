#include "launchpad/config/config_scanner.hpp"
#include <dirent.h>
#include <cstring>
#include <algorithm>

namespace launchpad::config {

std::vector<std::string> ConfigScanner::find_ini_files(const std::string& directory_path) {
    std::vector<std::string> files;
    DIR* dir = opendir(directory_path.c_str());
    if (!dir) {
        return files;
    }

    struct dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        size_t len = std::strlen(entry->d_name);
        if (len > 4 && std::strcmp(entry->d_name + len - 4, ".ini") == 0) {
            std::string full_path = directory_path;
            if (!full_path.empty() && full_path.back() != '/') {
                full_path.push_back('/');
            }
            full_path.append(entry->d_name);
            files.push_back(std::move(full_path));
        }
    }
    closedir(dir);

    // Sort alphabetically for deterministic precedence
    std::sort(files.begin(), files.end());
    return files;
}

void ConfigScanner::apply_settings(LaunchpadSettings& settings, const IniDocument& doc) {
    if (!doc.has_section("Settings")) {
        return;
    }
    if (auto val = doc.get_string("Settings", "Introducer"); val) {
        settings.introducer_key = *val;
    }
    if (auto val = doc.get_string("Settings", "Trailer"); val) {
        settings.trailer_key = *val;
    }
    if (auto val = doc.get_string("Settings", "ScriptDirectory"); val) {
        settings.script_directory = *val;
    }
    if (auto val = doc.get_string("Settings", "KeyboardInput"); val) {
        settings.keyboard_device = *val;
    }
    if (auto val = doc.get_string("Settings", "FivewayInput"); val) {
        settings.fiveway_device = *val;
    }

    int hot_interval = doc.get_int("Settings", "HotInterval", 0);
    if (hot_interval > 0) {
        settings.hot_interval = core::Milliseconds(static_cast<uint32_t>(hot_interval));
    }

    int key_delay = doc.get_int("Settings", "InterKeyDelay", 0);
    if (key_delay > 0) {
        settings.inter_key_delay = core::Milliseconds(static_cast<uint32_t>(key_delay));
    }
}

ScannedConfig ConfigScanner::scan_directory(const std::string& directory_path) {
    ScannedConfig result;
    auto ini_files = find_ini_files(directory_path);
    for (const auto& file_path : ini_files) {
        auto parsed = IniParser::parse_file(file_path);
        if (parsed.is_ok()) {
            const auto& doc = parsed.value();
            apply_settings(result.settings, doc);
            auto actions = doc.get_section_entries("Actions");
            for (auto& act : actions) {
                result.actions.push_back(std::move(act));
            }
        }
    }
    return result;
}

} // namespace launchpad::config
