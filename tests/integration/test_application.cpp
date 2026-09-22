#include "launchpad/system/application.hpp"
#include "launchpad/domain/key_definition.hpp"
#include "launchpad/hal/mock_input_source.hpp"
#include "launchpad/hal/mock_display.hpp"
#include "launchpad/hal/mock_command_runner.hpp"
#include "test_framework.hpp"
#include <filesystem>
#include <fstream>

using namespace launchpad::core;
using namespace launchpad::hal;
using namespace launchpad::system;
using namespace launchpad::domain::key_codes;

TEST_CASE("Integration - Application End-to-End Workflow") {
    std::string test_dir = "/tmp/test_lp_app";
    std::filesystem::create_directories(test_dir);
    std::string ini_file = test_dir + "/launchpad.ini";
    {
        std::ofstream ini(ini_file);
        ini << "[Settings]\n"
            << "hot_interval = 700\n"
            << "[Actions]\n"
            << "R = !echo testing_app\n";
    }

    MockInputSource input_source;
    MockDisplay display(DisplayBounds{600, 800, 4});
    MockCommandRunner cmd_runner;

    ApplicationServices services{
        input_source,
        display,
        cmd_runner,
        input_source // MockInputSource also implements IKeyInjector
    };

    Application app(services, test_dir);

    // Initial state: not grabbed
    CHECK_FALSE(input_source.is_grabbed());

    // 1. Introducer: Shift pressed and released
    input_source.queue_event(InputKeyEvent{KPKEY_SHIFT, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_SHIFT, launchpad::domain::KeyEventType::Released});
    app.step();

    // After introducer alone, input should be grabbed
    CHECK(input_source.is_grabbed());

    // 2. Next key: R pressed and released
    input_source.queue_event(InputKeyEvent{KPKEY_R, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_R, launchpad::domain::KeyEventType::Released});
    app.step();

    CHECK(input_source.is_grabbed());
    CHECK(cmd_runner.executed_commands().empty());

    // 3. Trailer: Enter pressed and released
    input_source.queue_event(InputKeyEvent{KPKEY_ENTER, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_ENTER, launchpad::domain::KeyEventType::Released});
    app.step();

    // After sequence complete: input grab released, command executed
    CHECK_FALSE(input_source.is_grabbed());
    REQUIRE(cmd_runner.executed_commands().size() == 1);
    CHECK(cmd_runner.executed_commands()[0] == "echo testing_app");

    std::filesystem::remove_all(test_dir);
}

TEST_CASE("Integration - Multi-Key Sequence (Shift Space)") {
    std::string test_dir = "/tmp/test_lp_app_multikey";
    std::filesystem::create_directories(test_dir);
    std::string ini_file = test_dir + "/launchpad.ini";
    {
        std::ofstream ini(ini_file);
        ini << "[Settings]\n"
            << "hot_interval = 700\n"
            << "[Actions]\n"
            << "Shift Space = !sync\n";
    }

    MockInputSource input_source;
    MockDisplay display(DisplayBounds{600, 800, 4});
    MockCommandRunner cmd_runner;

    ApplicationServices services{
        input_source,
        display,
        cmd_runner,
        input_source
    };

    Application app(services, test_dir);

    // 1. Introducer: Shift pressed & released
    input_source.queue_event(InputKeyEvent{KPKEY_SHIFT, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_SHIFT, launchpad::domain::KeyEventType::Released});
    app.step();
    CHECK(input_source.is_grabbed());

    // 2. First key in sequence: Shift pressed & released
    input_source.queue_event(InputKeyEvent{KPKEY_SHIFT, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_SHIFT, launchpad::domain::KeyEventType::Released});
    app.step();
    CHECK(input_source.is_grabbed());

    // 3. Second key in sequence: Space pressed & released
    input_source.queue_event(InputKeyEvent{KPKEY_SPACE, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_SPACE, launchpad::domain::KeyEventType::Released});
    app.step();
    CHECK(input_source.is_grabbed());

    // 4. Trailer: Enter pressed & released
    input_source.queue_event(InputKeyEvent{KPKEY_ENTER, launchpad::domain::KeyEventType::Pressed});
    app.step();
    input_source.queue_event(InputKeyEvent{KPKEY_ENTER, launchpad::domain::KeyEventType::Released});
    app.step();

    CHECK_FALSE(input_source.is_grabbed());
    REQUIRE(cmd_runner.executed_commands().size() == 1);
    CHECK(cmd_runner.executed_commands()[0] == "sync");

    std::filesystem::remove_all(test_dir);
}

