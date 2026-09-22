#pragma once

#include "launchpad/core/result.hpp"
#include <string_view>

namespace launchpad::system {

class Daemon {
public:
    static core::Result<void> daemonize(std::string_view log_path = "/dev/null");

private:
    static core::Result<void> fork_process();
    static core::Result<void> setup_session();
    static void redirect_standard_streams(std::string_view log_path);
};

} // namespace launchpad::system
