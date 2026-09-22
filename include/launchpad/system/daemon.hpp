/**
 * @file daemon.hpp
 * @brief Linux process daemonization and standard stream redirection.
 * @details Implements the standard double-fork or fork/setsid sequence, detaches from the
 *          controlling terminal, changes working directory, and redirects stdio streams.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include <string_view>

namespace launchpad::system {

/**
 * @brief Static utility handling Linux background process daemonization.
 */
class Daemon {
public:
    /**
     * @brief Transforms the calling process into a background system daemon.
     * @param[in] log_path Path for redirecting standard streams (defaults to /dev/null).
     * @return Result<void> indicating success or fork/session error.
     */
    static core::Result<void> daemonize(std::string_view log_path = "/dev/null");

private:
    static core::Result<void> fork_process();
    static core::Result<void> setup_session();
    static void redirect_standard_streams(std::string_view log_path);
};

} // namespace launchpad::system
