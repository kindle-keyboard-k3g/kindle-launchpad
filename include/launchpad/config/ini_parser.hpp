#pragma once

#include "launchpad/core/result.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>

namespace launchpad::config {

struct IniEntry {
    std::string key;
    std::string value;
};

class IniDocument {
public:
    void add_entry(const std::string& section, std::string key, std::string value);

    [[nodiscard]] bool has_section(std::string_view section) const noexcept;
    [[nodiscard]] std::optional<std::string> get_string(std::string_view section, std::string_view key) const;
    [[nodiscard]] int get_int(std::string_view section, std::string_view key, int default_value = 0) const;

    [[nodiscard]] const std::vector<IniEntry>& get_section_entries(std::string_view section) const;
    [[nodiscard]] std::vector<std::string> get_section_names() const;

private:
    static std::string normalize_key(std::string_view s);

    std::unordered_map<std::string, std::vector<IniEntry>> sections_;
    std::vector<std::string> section_order_;
};

class IniParser {
public:
    static core::Result<IniDocument> parse_string(std::string_view content);
    static core::Result<IniDocument> parse_file(const std::string& file_path);

    static std::string_view trim(std::string_view str);
    static bool is_comment(char c) noexcept;
};

} // namespace launchpad::config
