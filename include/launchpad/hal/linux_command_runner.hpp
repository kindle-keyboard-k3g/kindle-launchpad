#pragma once

#include "launchpad/hal/command_runner.hpp"

namespace launchpad::hal {

class LinuxCommandRunner : public ICommandRunner {
public:
    int run_command(std::string_view command) override;
};

} // namespace launchpad::hal
