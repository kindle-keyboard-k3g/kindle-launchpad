#include "launchpad/domain/action.hpp"
#include <sstream>

namespace launchpad::domain {

std::string ActionParser::build_script_path(std::string_view script, std::string_view script_dir) {
    if (script.empty()) {
        return "";
    }
    if (script.front() == '/' || script_dir.empty()) {
        return std::string(script);
    }
    if (script_dir.back() == '/') {
        return std::string(script_dir) + std::string(script);
    }
    return std::string(script_dir) + "/" + std::string(script);
}

Action ActionParser::parse_command(std::string_view command_str, std::string display_name, std::string_view script_dir) {
    if (command_str.empty()) {
        return Action{std::move(display_name), SendKeysAction{""}};
    }
    char prefix = command_str.front();
    if (prefix == '!') {
        return Action{std::move(display_name), ShellAction{std::string(command_str.substr(1))}};
    }
    if (prefix == '@') {
        auto path = build_script_path(command_str.substr(1), script_dir);
        return Action{std::move(display_name), ScriptAction{std::move(path)}};
    }
    if (prefix == '#') {
        return Action{std::move(display_name), KindleKeysAction{std::string(command_str.substr(1))}};
    }
    if (prefix == '\\') {
        return Action{std::move(display_name), SendKeysAction{std::string(command_str.substr(1))}};
    }
    return Action{std::move(display_name), SendKeysAction{std::string(command_str)}};
}

std::optional<core::KeySequence> ActionParser::parse_sequence(std::string_view sequence_str, const KeyTable& key_table) {
    std::istringstream stream{std::string(sequence_str)};
    std::string token;
    core::KeySequence sequence;

    while (stream >> token) {
        auto code = key_table.find_code(token);
        if (!code.has_value()) {
            return std::nullopt;
        }
        sequence.push(code.value());
    }
    if (sequence.empty()) {
        return std::nullopt;
    }
    return sequence;
}

} // namespace launchpad::domain
