/**
 * @file cli_options.hpp
 * @brief Command-line option parser for Kindle Launchpad.
 * @details Parses command-line flags (-d for foreground debug, -kb for keyboard node,
 *          and -fw for 5-way joystick node) into structured options.
 */

#pragma once

#include <string>
#include <string_view>

namespace launchpad::system {

/**
 * @brief Explicit device path overrides supplied via command-line flags.
 */
struct DevicePaths {
    std::string keyboard; ///< Path to keyboard device node override.
    std::string fiveway;  ///< Path to 5-way controller device node override.
};

/**
 * @brief Parsed command-line arguments configuring runtime daemon behavior.
 */
class CliOptions {
public:
    /**
     * @brief Checks if Launchpad should run in the foreground without daemonizing.
     * @return True if -d flag was specified.
     */
    [[nodiscard]] bool is_foreground() const noexcept {
        return foreground_;
    }

    /**
     * @brief Returns the keyboard device path override, if any.
     * @return Path string or empty if using default.
     */
    [[nodiscard]] const std::string& keyboard_device() const noexcept {
        return devices_.keyboard;
    }

    /**
     * @brief Returns the 5-way controller device path override, if any.
     * @return Path string or empty if using default.
     */
    [[nodiscard]] const std::string& fiveway_device() const noexcept {
        return devices_.fiveway;
    }

    /**
     * @brief Parses command-line arguments into a CliOptions instance.
     * @param[in] argc Argument count from main().
     * @param[in] argv Argument array from main().
     * @return Populated CliOptions instance.
     */
    static CliOptions parse(int argc, char* const argv[]);

private:
    DevicePaths devices_{};
    bool foreground_{false};

    static void parse_flag(CliOptions& options, std::string_view argument);
};

} // namespace launchpad::system
