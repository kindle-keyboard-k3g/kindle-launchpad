#pragma once

#include <string_view>

namespace launchpad::hal {

class ICommandRunner {
public:
    virtual ~ICommandRunner() = default;

    virtual int run_command(std::string_view command) = 0;
};

} // namespace launchpad::hal
