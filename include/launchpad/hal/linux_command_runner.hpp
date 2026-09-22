/**
 * @file linux_command_runner.hpp
 * @brief Concrete Linux implementation of ICommandRunner using /bin/sh.
 * @details Executes system commands asynchronously in the background via POSIX system().
 */

#pragma once

#include "launchpad/hal/command_runner.hpp"

namespace launchpad::hal {

/**
 * @brief Concrete command runner executing shell commands using Linux system calls.
 */
class LinuxCommandRunner : public ICommandRunner {
public:
    /**
     * @brief Spawns the specified shell command string using /bin/sh -c "<cmd> &".
     * @param[in] command Shell command line to execute.
     * @return Process return code or 0 on background spawn.
     */
    int run_command(std::string_view command) override;
};

} // namespace launchpad::hal
