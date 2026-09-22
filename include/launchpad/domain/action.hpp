/**
 * @file action.hpp
 * @brief Domain models for executable hotkey actions and commands.
 * @details Represents the discriminated union (std::variant) of all action types supported
 *          by Launchpad: shell commands, scripts, search keys, virtual keystrokes, and screenshots.
 */

#pragma once

#include "launchpad/core/units.hpp"
#include "launchpad/domain/key_definition.hpp"
#include <string>
#include <string_view>
#include <variant>
#include <optional>

namespace launchpad::domain {

/**
 * @brief Executes an arbitrary system shell command asynchronously.
 * @details Corresponds to the '!' action prefix in INI files.
 */
struct ShellAction {
    std::string command; ///< Shell command line to pass to /bin/sh -c.
    auto operator<=>(const ShellAction&) const = default;
};

/**
 * @brief Executes a Kindle framework shell script from the configured script directory.
 * @details Corresponds to the '@' action prefix in INI files.
 */
struct ScriptAction {
    std::string script_path; ///< Full path or relative filename of the executable script.
    auto operator<=>(const ScriptAction&) const = default;
};

/**
 * @brief Injects keystrokes into the Kindle native search bar.
 * @details Corresponds to the '#' action prefix in INI files.
 */
struct KindleKeysAction {
    std::string keys; ///< Text string to enter into the Kindle framework search bar.
    auto operator<=>(const KindleKeysAction&) const = default;
};

/**
 * @brief Injects simulated raw keystrokes into the Kindle input driver (/proc/keypad).
 * @details Corresponds to quoted strings or raw tokens in INI files.
 */
struct SendKeysAction {
    std::string keys; ///< Space-separated key tokens or quoted character string.
    auto operator<=>(const SendKeysAction&) const = default;
};

/**
 * @brief Captures the e-ink framebuffer and writes an uncompressed BMP image to disk.
 * @details Triggered by the default triple-shift sequence or empty action entries.
 */
struct ScreenshotAction {
    auto operator<=>(const ScreenshotAction&) const = default;
};

/**
 * @brief Discriminated union of all possible executable action payloads.
 */
using ActionPayload = std::variant<ShellAction, ScriptAction, KindleKeysAction, SendKeysAction, ScreenshotAction>;

/**
 * @brief Complete action entity pairing a human-readable display label with an executable payload.
 */
struct Action {
    std::string display_name; ///< Human-readable label displayed on the status bar (e.g. "Success!").
    ActionPayload payload;    ///< Specific action data variant to execute.
};

/**
 * @brief Parser converting raw INI configuration lines into Action and KeySequence domain entities.
 */
class ActionParser {
public:
    /**
     * @brief Parses an action command line into a typed Action entity.
     * @param[in] command_str Raw action string (e.g. "!reboot", "@toggle_wifi.sh", "\"calibre\"").
     * @param[in] display_name Label to show on the status bar.
     * @param[in] script_dir Directory prepended to script actions.
     * @return Fully populated Action entity.
     */
    static Action parse_command(std::string_view command_str, std::string display_name, std::string_view script_dir);

    /**
     * @brief Parses a space-delimited hotkey sequence string into a KeySequence.
     * @param[in] sequence_str Key chord string (e.g. "Shift Shift Space" or "Shift Enter").
     * @param[in] key_table KeyTable used to translate symbolic key names into raw key codes.
     * @return Optional containing the resolved KeySequence, or std::nullopt if an unknown key was encountered.
     */
    static std::optional<core::KeySequence> parse_sequence(std::string_view sequence_str, const KeyTable& key_table);

private:
    static std::string build_script_path(std::string_view script, std::string_view script_dir);
};

} // namespace launchpad::domain
