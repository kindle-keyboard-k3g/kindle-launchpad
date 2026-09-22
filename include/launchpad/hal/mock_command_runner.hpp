/**
 * @file mock_command_runner.hpp
 * @brief In-memory mock command runner for host testing.
 * @details Implements ICommandRunner test double, intercepting shell command execution
 *          and capturing executed command strings in memory for test assertions.
 */

#pragma once

#include "launchpad/hal/command_runner.hpp"
#include <string>
#include <vector>

namespace launchpad::hal {

/**
 * @brief Test double recording shell commands executed during tests without spawning processes.
 */
class MockCommandRunner : public ICommandRunner {
public:
    /**
     * @brief Constructs a MockCommandRunner with the given default return code.
     * @param[in] default_return_code Simulated exit code (defaults to 0 success).
     */
    explicit MockCommandRunner(int default_return_code = 0) noexcept
        : return_code_(default_return_code) {}

    /**
     * @brief Records command string into execution history and returns configured return code.
     * @param[in] command Command string that was executed.
     * @return Simulated return code integer.
     */
    int run_command(std::string_view command) override {
        executed_commands_.emplace_back(command);
        return return_code_;
    }

    /**
     * @brief Configures the exit status code returned by subsequent run_command() calls.
     * @param[in] code Exit status integer.
     */
    void set_return_code(int code) noexcept {
        return_code_ = code;
    }

    /**
     * @brief Returns the list of all command lines executed through this runner.
     * @return Const reference to vector of executed command strings.
     */
    [[nodiscard]] const std::vector<std::string>& executed_commands() const noexcept {
        return executed_commands_;
    }

    /**
     * @brief Clears the history of executed commands.
     */
    void clear() noexcept {
        executed_commands_.clear();
    }

private:
    int return_code_{0};
    std::vector<std::string> executed_commands_;
};

} // namespace launchpad::hal
