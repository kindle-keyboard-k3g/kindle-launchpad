/**
 * @file ini_parser.hpp
 * @brief Typesafe, pure-logic INI file parser for Kindle Launchpad.
 * @details Implements resilient parsing of standard INI formats, preserving section order,
 *          supporting comments (';' and '#'), and handling whitespace trimming.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>

namespace launchpad::config {

/**
 * @brief Represents a single key-value assignment within an INI section.
 */
struct IniEntry {
    std::string key;   ///< Configuration property name.
    std::string value; ///< Raw string value assigned to the key.
};

/**
 * @brief In-memory representation of a parsed INI document organized into sections.
 */
class IniDocument {
public:
    /**
     * @brief Appends a key-value entry to the specified section.
     * @param[in] section Name of the section (e.g. "Settings" or "Actions").
     * @param[in] key Property key name.
     * @param[in] value Value string.
     */
    void add_entry(const std::string& section, std::string key, std::string value);

    /**
     * @brief Checks if a section exists in the document.
     * @param[in] section Section name to query.
     * @return True if section is present, false otherwise.
     */
    [[nodiscard]] bool has_section(std::string_view section) const noexcept;

    /**
     * @brief Retrieves a string property value from a section.
     * @param[in] section Section name to query.
     * @param[in] key Key name within the section.
     * @return Optional containing the string value if found, or std::nullopt.
     */
    [[nodiscard]] std::optional<std::string> get_string(std::string_view section, std::string_view key) const;

    /**
     * @brief Retrieves an integer property value from a section with a fallback default.
     * @param[in] section Section name to query.
     * @param[in] key Key name within the section.
     * @param[in] default_value Fallback value returned if key is missing or unparseable.
     * @return Parsed integer or default_value.
     */
    [[nodiscard]] int get_int(std::string_view section, std::string_view key, int default_value = 0) const;

    /**
     * @brief Returns all entries belonging to the specified section in order.
     * @param[in] section Section name.
     * @return Const reference to the vector of entries. Returns static empty vector if missing.
     */
    [[nodiscard]] const std::vector<IniEntry>& get_section_entries(std::string_view section) const;

    /**
     * @brief Returns the names of all sections in the order they were parsed.
     * @return Vector of section name strings.
     */
    [[nodiscard]] std::vector<std::string> get_section_names() const;

private:
    static std::string normalize_key(std::string_view s);

    std::unordered_map<std::string, std::vector<IniEntry>> sections_;
    std::vector<std::string> section_order_;
};

/**
 * @brief Static parser transforming raw INI text or files into IniDocument instances.
 */
class IniParser {
public:
    /**
     * @brief Parses an INI configuration from an in-memory string.
     * @param[in] content Raw INI file text.
     * @return Result containing the parsed IniDocument or an Error describing syntax defects.
     */
    static core::Result<IniDocument> parse_string(std::string_view content);

    /**
     * @brief Reads and parses an INI configuration from the filesystem.
     * @param[in] file_path Filesystem path to the .ini file.
     * @return Result containing the parsed IniDocument or an I/O / parse Error.
     */
    static core::Result<IniDocument> parse_file(const std::string& file_path);

    /**
     * @brief Trims leading and trailing whitespace from a string view.
     * @param[in] str Input text.
     * @return Trimmed string view.
     */
    static std::string_view trim(std::string_view str);

    /**
     * @brief Checks if a character indicates a comment line (';' or '#').
     * @param[in] c Character to inspect.
     * @return True if character begins a comment, false otherwise.
     */
    static bool is_comment(char c) noexcept;
};

} // namespace launchpad::config
