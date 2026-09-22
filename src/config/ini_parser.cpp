#include "launchpad/config/ini_parser.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace launchpad::config {

std::string IniDocument::normalize_key(std::string_view s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

void IniDocument::add_entry(const std::string& section, std::string key, std::string value) {
    std::string norm_sec = normalize_key(section);
    if (!sections_.contains(norm_sec)) {
        section_order_.push_back(section);
    }
    sections_[norm_sec].push_back(IniEntry{std::move(key), std::move(value)});
}

bool IniDocument::has_section(std::string_view section) const noexcept {
    return sections_.contains(normalize_key(section));
}

std::optional<std::string> IniDocument::get_string(std::string_view section, std::string_view key) const {
    std::string norm_sec = normalize_key(section);
    auto it = sections_.find(norm_sec);
    if (it == sections_.end()) {
        return std::nullopt;
    }
    std::string norm_k = normalize_key(key);
    for (const auto& entry : it->second) {
        if (normalize_key(entry.key) == norm_k) {
            return entry.value;
        }
    }
    return std::nullopt;
}

int IniDocument::get_int(std::string_view section, std::string_view key, int default_value) const {
    auto val = get_string(section, key);
    if (!val.has_value()) {
        return default_value;
    }
    try {
        return std::stoi(*val);
    } catch (...) {
        return default_value;
    }
}

static const std::vector<IniEntry> EMPTY_ENTRIES;

const std::vector<IniEntry>& IniDocument::get_section_entries(std::string_view section) const {
    auto it = sections_.find(normalize_key(section));
    if (it == sections_.end()) {
        return EMPTY_ENTRIES;
    }
    return it->second;
}

std::vector<std::string> IniDocument::get_section_names() const {
    return section_order_;
}

std::string_view IniParser::trim(std::string_view str) {
    while (!str.empty() && std::isspace(static_cast<unsigned char>(str.front()))) {
        str.remove_prefix(1);
    }
    while (!str.empty() && std::isspace(static_cast<unsigned char>(str.back()))) {
        str.remove_suffix(1);
    }
    return str;
}

bool IniParser::is_comment(char c) noexcept {
    return c == ';' || c == '#';
}

static void parse_line_entry(IniDocument& doc, std::string& current_section, std::string_view line) {
    if (line.empty() || IniParser::is_comment(line.front())) {
        return;
    }
    if (line.front() == '[' && line.back() == ']') {
        current_section = std::string(IniParser::trim(line.substr(1, line.size() - 2)));
        return;
    }
    size_t eq_pos = line.find('=');
    if (eq_pos == std::string_view::npos) {
        return;
    }
    std::string key = std::string(IniParser::trim(line.substr(0, eq_pos)));
    std::string val = std::string(IniParser::trim(line.substr(eq_pos + 1)));
    doc.add_entry(current_section, std::move(key), std::move(val));
}

core::Result<IniDocument> IniParser::parse_string(std::string_view content) {
    IniDocument doc;
    std::string current_section = "default";
    size_t start = 0;
    while (start < content.size()) {
        size_t end = content.find('\n', start);
        if (end == std::string_view::npos) {
            end = content.size();
        }
        std::string_view line = trim(content.substr(start, end - start));
        parse_line_entry(doc, current_section, line);
        start = end + 1;
    }
    return doc;
}

core::Result<IniDocument> IniParser::parse_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return core::Error("Failed to open INI file: " + file_path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parse_string(buffer.str());
}

} // namespace launchpad::config
