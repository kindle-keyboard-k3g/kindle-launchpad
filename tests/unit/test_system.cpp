#include "launchpad/system/cli_options.hpp"
#include "launchpad/system/signal_handler.hpp"
#include "launchpad/system/pid_lock.hpp"
#include "launchpad/system/rce_server.hpp"
#include "launchpad/hal/mock_command_runner.hpp"
#include "test_framework.hpp"
#include <cstdio>

using namespace launchpad::core;
using namespace launchpad::system;

TEST_CASE("System - CliOptions Parsing") {
    SECTION("Default options") {
        char* argv[] = {const_cast<char*>("launchpad")};
        auto opts = CliOptions::parse(1, argv);
        CHECK_FALSE(opts.is_foreground());
        CHECK(opts.keyboard_device().empty());
        CHECK(opts.fiveway_device().empty());
    }

    SECTION("Foreground flag (-d)") {
        char* argv[] = {const_cast<char*>("launchpad"), const_cast<char*>("-d")};
        auto opts = CliOptions::parse(2, argv);
        CHECK(opts.is_foreground());
    }

    SECTION("Custom devices (-kb and -fw)") {
        char* argv[] = {
            const_cast<char*>("launchpad"),
            const_cast<char*>("-d"),
            const_cast<char*>("-kb/dev/input/event0"),
            const_cast<char*>("-fw/dev/input/event1")
        };
        auto opts = CliOptions::parse(4, argv);
        CHECK(opts.is_foreground());
        CHECK(opts.keyboard_device() == "/dev/input/event0");
        CHECK(opts.fiveway_device() == "/dev/input/event1");
    }
}

TEST_CASE("System - SignalHandler") {
    SignalHandler::install();
    CHECK_FALSE(SignalHandler::is_shutdown_requested());
    CHECK_FALSE(SignalHandler::is_reload_requested());

    SignalHandler::request_shutdown();
    CHECK(SignalHandler::is_shutdown_requested());
}

TEST_CASE("System - PidLock") {
    std::string lock_path = "/tmp/test_launchpad.pid";
    std::remove(lock_path.c_str());

    {
        PidLock lock1(lock_path);
        auto res1 = lock1.acquire();
        CHECK(res1.is_ok());
        CHECK(lock1.is_locked());

        // Second lock attempt on same file should fail
        PidLock lock2(lock_path);
        auto res2 = lock2.acquire();
        CHECK(res2.is_err());
        CHECK_FALSE(lock2.is_locked());
    }

    // After lock1 out of scope, lock3 can acquire
    {
        PidLock lock3(lock_path);
        auto res3 = lock3.acquire();
        CHECK(res3.is_ok());
        CHECK(lock3.is_locked());
    }

    std::remove(lock_path.c_str());
}

TEST_CASE("System - RceServer Message Handling") {
    launchpad::hal::MockCommandRunner runner;
    RceConfig config;
    config.auto_enable = false;
    config.enable_string = "LP_ENABLE";
    config.disable_string = "LP_DISABLE";

    RceServer server(config, runner);
    bool enabled = false;

    // Disabled: commands should not run
    server.process_message("reboot", enabled);
    CHECK_FALSE(enabled);
    CHECK(runner.executed_commands().empty());

    // Send enable string: should enable
    server.process_message("LP_ENABLE", enabled);
    CHECK(enabled);
    CHECK(runner.executed_commands().empty());

    // Enabled: command should execute
    server.process_message("echo hi", enabled);
    CHECK(enabled);
    REQUIRE(runner.executed_commands().size() == 1);
    CHECK(runner.executed_commands()[0] == "echo hi");

    // Send disable string: should disable
    server.process_message("LP_DISABLE", enabled);
    CHECK_FALSE(enabled);

    // Disabled again: command should not execute
    server.process_message("echo bye", enabled);
    CHECK(runner.executed_commands().size() == 1);
}

