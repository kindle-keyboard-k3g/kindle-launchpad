/**
 * @file config_scanner.hpp
 * @brief Multi-file INI configuration scanner and aggregator.
 * @details Discovers and aggregates multiple INI files from the Launchpad configuration
 *          directory, prioritizing launchpad.ini while loading modular action files.
 */

#pragma once

#include "launchpad/config/ini_parser.hpp"
#include "launchpad/config/settings.hpp"
#include <string>
#include <vector>

namespace launchpad::config {

/**
 * @brief Aggregated configuration state holding combined settings and action mappings.
 */
struct ScannedConfig {
    LaunchpadSettings settings;     ///< Merged daemon settings from all scanned INI files.
    std::vector<IniEntry> actions;  ///< Combined list of key-action entries from all [Actions] sections.
};

/**
 * @brief Scanner that discovers and processes all INI configuration files in a directory.
 */
class ConfigScanner {
public:
    /**
     * @brief Scans the specified directory for all *.ini files and merges their configurations.
     * @details Parses launchpad.ini first (if present) to establish baseline settings,
     *          then parses all other *.ini files alphabetically, appending hotkey actions.
     * @param[in] directory_path Filesystem directory to search (e.g. /mnt/us/launchpad).
     * @return ScannedConfig containing consolidated settings and hotkey actions.
     */
    static ScannedConfig scan_directory(const std::string& directory_path);

private:
    static std::vector<std::string> find_ini_files(const std::string& directory_path);
    static void apply_settings(LaunchpadSettings& settings, const IniDocument& doc);
};

} // namespace launchpad::config
