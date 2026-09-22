#pragma once

#include "launchpad/hal/command_runner.hpp"
#include <string>
#include <vector>

namespace launchpad::hal {

class MockCommandRunner : public ICommandRunner {
public:
    explicit MockCommandRunner(int default_return_code = 0) noexcept
        : return_code_(default_return_code) {}

    int run_command(std::string_view command) override {
        executed_commands_.emplace_back(command);
        return return_code_;
    }

    void set_return_code(int code) noexcept {
        return_code_ = code;
    }

    [[nodiscard]] const std::vector<std::string>& executed_commands() const noexcept {
        return executed_commands_;
    }

    void clear() noexcept {
        executed_commands_.clear();
    }

private:
    int return_code_{0};
    std::vector<std::string> executed_commands_;
};

} // namespace launchpad::hal
