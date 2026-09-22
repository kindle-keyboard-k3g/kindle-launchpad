/**
 * @file command_runner.hpp
 * @brief Abstract interface for executing system commands and shell scripts.
 * @details Decouples action dispatchers from POSIX fork/exec/system system calls,
 *          enabling verification in mock-based host unit tests.
 */

#pragma once

#include <string_view>

namespace launchpad::hal {

/**
 * @brief Abstract command runner interface for spawning system shell processes.
 */
class ICommandRunner {
public:
    virtual ~ICommandRunner() = default;

    /**
     * @brief Executes a shell command line asynchronously or synchronously.
     * @param[in] command Full shell command string to execute.
     * @return Exit status code or 0 on successful asynchronous spawn.
     */
    virtual int run_command(std::string_view command) = 0;
};

} // namespace launchpad::hal
