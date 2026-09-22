#include "launchpad/system/application.hpp"
#include "launchpad/system/cli_options.hpp"
#include "launchpad/system/signal_handler.hpp"
#include "launchpad/system/daemon.hpp"
#include "launchpad/system/pid_lock.hpp"
#include "launchpad/hal/evdev_input_source.hpp"
#include "launchpad/hal/kindle_display.hpp"
#include "launchpad/hal/linux_command_runner.hpp"
#include "launchpad/hal/linux_key_injector.hpp"
#include <filesystem>
#include <iostream>

namespace {

std::string get_executable_directory() {
    std::error_code error;
    auto exe_path = std::filesystem::read_symlink("/proc/self/exe", error);
    if (!error && !exe_path.empty()) {
        return exe_path.parent_path().string();
    }
    return ".";
}

std::vector<std::string> build_input_devices(const launchpad::system::CliOptions& options) {
    std::string kbd = options.keyboard_device().empty() ? "/dev/input/event0" : options.keyboard_device();
    std::string fw = options.fiveway_device().empty() ? "/dev/input/event1" : options.fiveway_device();
    return {kbd, fw, "/dev/input/event2"};
}

} // namespace

int main(int argc, char* argv[]) {
    auto options = launchpad::system::CliOptions::parse(argc, argv);
    std::string config_dir = get_executable_directory();
    launchpad::system::PidLock pid_lock("/var/run/launchpad.pid");

    if (!options.is_foreground()) {
        static_cast<void>(launchpad::system::Daemon::daemonize(config_dir + "/launchpad.log"));
        static_cast<void>(pid_lock.acquire());
    }

    launchpad::system::SignalHandler::install();

    auto input_paths = build_input_devices(options);
    launchpad::hal::EvdevInputSource input_source(input_paths);
    launchpad::hal::KindleDisplay display("/dev/fb0");
    launchpad::hal::LinuxCommandRunner command_runner;
    launchpad::hal::LinuxKeyInjector key_injector;

    launchpad::system::ApplicationServices services{
        input_source,
        display,
        command_runner,
        key_injector
    };

    launchpad::system::Application app(services, config_dir);
    return app.run();
}
