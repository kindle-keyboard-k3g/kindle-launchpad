#include "launchpad/hal/input_source.hpp"
#include "launchpad/hal/mock_input_source.hpp"
#include "launchpad/hal/mock_command_runner.hpp"
#include "launchpad/hal/mock_display.hpp"
#include "test_framework.hpp"

using namespace launchpad::core;
using namespace launchpad::hal;
using namespace launchpad::domain;

TEST_CASE("HAL - MockInputSource and ScopedInputGrab") {
    MockInputSource input;

    SECTION("Initial state is ungrabbed and empty") {
        CHECK_FALSE(input.is_grabbed());
        auto events = input.poll_events(Milliseconds(10));
        CHECK(events.empty());
    }

    SECTION("Queuing and polling events") {
        input.queue_event(InputKeyEvent{42, KeyEventType::Pressed});
        input.queue_event(InputKeyEvent{42, KeyEventType::Released});

        auto events = input.poll_events(Milliseconds(10));
        REQUIRE(events.size() == 2);
        CHECK(events[0].code == 42);
        CHECK(events[0].type == KeyEventType::Pressed);
        CHECK(events[1].code == 42);
        CHECK(events[1].type == KeyEventType::Released);

        // Next poll should be empty
        auto empty_events = input.poll_events(Milliseconds(10));
        CHECK(empty_events.empty());
    }

    SECTION("ScopedInputGrab RAII guarantees release") {
        CHECK_FALSE(input.is_grabbed());
        {
            ScopedInputGrab grab(input);
            CHECK(grab.is_grabbed());
            CHECK(input.is_grabbed());
        }
        CHECK_FALSE(input.is_grabbed());
    }

    SECTION("Key injection recording") {
        CHECK(input.injected_keys().empty());
        auto res = input.inject_key(57, SendMode::Normal);
        CHECK(res.is_ok());
        REQUIRE(input.injected_keys().size() == 1);
        CHECK(input.injected_keys()[0].code == 57);
        CHECK(input.injected_keys()[0].mode == SendMode::Normal);
    }
}

TEST_CASE("HAL - MockCommandRunner") {
    MockCommandRunner runner(0);

    SECTION("Runs command and records execution") {
        int rc = runner.run_command("sync");
        CHECK(rc == 0);
        REQUIRE(runner.executed_commands().size() == 1);
        CHECK(runner.executed_commands()[0] == "sync");
    }

    SECTION("Configurable return code") {
        runner.set_return_code(127);
        int rc = runner.run_command("nonexistent");
        CHECK(rc == 127);
    }
}

TEST_CASE("HAL - MockDisplay") {
    DisplayBounds bounds{100, 50, 4};
    MockDisplay display(bounds);

    SECTION("Initializes with specified bounds and clear color") {
        CHECK(display.bounds().width == 100);
        CHECK(display.bounds().height == 50);
        CHECK(display.get_pixel(10, 10) == 0xFF);
    }

    SECTION("Sets and gets pixels") {
        display.set_pixel(10, 10, 0x00);
        CHECK(display.get_pixel(10, 10) == 0x00);
        CHECK(display.get_pixel(11, 10) == 0xFF);
    }

    SECTION("Tracks update regions") {
        UpdateRegion reg{5, 10, 20, 30};
        auto res = display.update(reg);
        CHECK(res.is_ok());
        REQUIRE(display.update_regions().size() == 1);
        CHECK(display.update_regions()[0].x == 5);
        CHECK(display.update_regions()[0].y == 10);
    }
}
