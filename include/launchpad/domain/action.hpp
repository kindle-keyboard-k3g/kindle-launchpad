#pragma once

#include "launchpad/core/units.hpp"
#include "launchpad/domain/key_definition.hpp"
#include <string>
#include <string_view>
#include <variant>
#include <optional>

namespace launchpad::domain {

struct ShellAction {
    std::string command;
    auto operator<=>(const ShellAction&) const = default;
};

struct ScriptAction {
    std::string script_path;
    auto operator<=>(const ScriptAction&) const = default;
};

struct KindleKeysAction {
    std::string keys;
    auto operator<=>(const KindleKeysAction&) const = default;
};

struct SendKeysAction {
    std::string keys;
    auto operator<=>(const SendKeysAction&) const = default;
};

struct ScreenshotAction {
    auto operator<=>(const ScreenshotAction&) const = default;
};

using ActionPayload = std::variant<ShellAction, ScriptAction, KindleKeysAction, SendKeysAction, ScreenshotAction>;

struct Action {
    std::string display_name;
    ActionPayload payload;
};

class ActionParser {
public:
    static Action parse_command(std::string_view command_str, std::string display_name, std::string_view script_dir);
    static std::optional<core::KeySequence> parse_sequence(std::string_view sequence_str, const KeyTable& key_table);

private:
    static std::string build_script_path(std::string_view script, std::string_view script_dir);
};

} // namespace launchpad::domain
