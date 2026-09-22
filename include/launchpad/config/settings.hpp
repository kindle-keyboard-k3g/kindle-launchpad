#pragma once

#include "launchpad/core/units.hpp"
#include <string>

namespace launchpad::config {

struct LaunchpadSettings {
    std::string introducer_key{"Shift"};
    std::string trailer_key{"Enter"};
    core::Milliseconds hot_interval{700};
    core::Milliseconds inter_key_delay{300};
    std::string script_directory{"./scripts"};
    std::string keyboard_device{"/dev/input/event0"};
    std::string fiveway_device{"/dev/input/event1"};
    std::string volume_device{"/dev/input/event2"};

    // Remote Command Entry settings
    bool rce_enabled{false};
    uint16_t rce_port{4444};
    std::string rce_group{"224.0.0.1"};
    std::string rce_interface{"eth0"};
};

} // namespace launchpad::config
