/**
 * @file settings.hpp
 * @brief Authoritative configuration data structure for Kindle Launchpad.
 * @details Models the global [Settings] section found in launchpad.ini, including
 *          hotkey state machine timings, hardware event paths, and Remote Command Entry (RCE).
 */

#pragma once

#include "launchpad/core/units.hpp"
#include <string>

namespace launchpad::config {

/**
 * @brief Global daemon configuration options parsed from the [Settings] section of INI files.
 */
struct LaunchpadSettings {
    std::string introducer_key{"Shift"};          ///< Key that begins a hotkey sequence (default: Shift).
    std::string trailer_key{"Enter"};             ///< Key that terminates and executes a sequence (default: Enter).
    core::Milliseconds hot_interval{700};         ///< Maximum timeout in ms between keystrokes before sequence resets.
    core::Milliseconds inter_key_delay{300};      ///< Delay in ms between simulated keystrokes.
    std::string script_directory{"./scripts"};    ///< Path to directory containing framework shell scripts.
    std::string keyboard_device{"/dev/input/event0"}; ///< Linux evdev node for physical keyboard.
    std::string fiveway_device{"/dev/input/event1"};  ///< Linux evdev node for five-way controller.
    std::string volume_device{"/dev/input/event2"};   ///< Linux evdev node for volume rocker.

    // Remote Command Entry settings
    bool rce_enabled{false};                      ///< Whether UDP multicast remote command receiver is active.
    uint16_t rce_port{4444};                      ///< Multicast port for RCE packets.
    std::string rce_group{"224.0.0.1"};           ///< Multicast group IP address.
    std::string rce_interface{"eth0"};            ///< Network interface bound for multicast reception.
};

} // namespace launchpad::config
