#include "launchpad/system/cli_options.hpp"

namespace launchpad::system {

void CliOptions::parse_flag(CliOptions& options, std::string_view argument) {
    if (argument == "-d") {
        options.foreground_ = true;
        return;
    }
    if (argument.starts_with("-kb")) {
        options.devices_.keyboard = std::string(argument.substr(3));
        return;
    }
    if (argument.starts_with("-fw")) {
        options.devices_.fiveway = std::string(argument.substr(3));
        return;
    }
}

CliOptions CliOptions::parse(int argc, char* const argv[]) {
    CliOptions options;
    for (int index = 1; index < argc; ++index) {
        parse_flag(options, argv[index]);
    }
    return options;
}

} // namespace launchpad::system
