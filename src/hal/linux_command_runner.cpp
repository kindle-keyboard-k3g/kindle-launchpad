#include "launchpad/hal/linux_command_runner.hpp"
#include <cstdlib>
#include <string>

namespace launchpad::hal {

int LinuxCommandRunner::run_command(std::string_view command) {
    if (command.empty()) {
        return 0;
    }
    std::string cmd(command);
    return std::system(cmd.c_str());
}

} // namespace launchpad::hal
