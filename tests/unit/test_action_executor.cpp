#include "launchpad/domain/action_executor.hpp"
#include "launchpad/hal/mock_command_runner.hpp"
#include "launchpad/hal/mock_input_source.hpp"
#include "test_framework.hpp"

using namespace launchpad::core;
using namespace launchpad::domain;
using namespace launchpad::hal;
using namespace launchpad::domain::key_codes;

class MockStatusNotifier : public IStatusNotifier {
public:
    void show_status(std::string_view message) override {
        status_messages.emplace_back(message);
    }

    void show_result(bool success) override {
        results.push_back(success);
    }

    void hide_status() override {
        hide_count++;
    }

    void clear() {
        status_messages.clear();
        results.clear();
        hide_count = 0;
    }

    std::vector<std::string> status_messages;
    std::vector<bool> results;
    int hide_count{0};
};

TEST_CASE("ActionExecutor - ShellAction Success") {
    MockCommandRunner cmd_runner;
    MockInputSource input_source;
    MockStatusNotifier notifier;
    KeyTable key_table;
    AsciiTranslator ascii_translator;

    ActionExecutor executor(cmd_runner, input_source, key_table, ascii_translator, &notifier);

    Action action{"Shift R", ShellAction{"reboot"}};
    auto res = executor.execute(action);
    CHECK(res.is_ok());

    REQUIRE(cmd_runner.executed_commands().size() == 1);
    CHECK(cmd_runner.executed_commands()[0] == "reboot");

    REQUIRE(notifier.status_messages.size() == 1);
    CHECK(notifier.status_messages[0] == "^[Shift R]");

    REQUIRE(notifier.results.size() == 1);
    CHECK(notifier.results[0] == true); // rc == 0
    CHECK(notifier.hide_count == 1);
}

TEST_CASE("ActionExecutor - ShellAction Failure") {
    MockCommandRunner cmd_runner;
    MockInputSource input_source;
    MockStatusNotifier notifier;
    KeyTable key_table;
    AsciiTranslator ascii_translator;

    ActionExecutor executor(cmd_runner, input_source, key_table, ascii_translator, &notifier);

    cmd_runner.set_return_code(1);
    Action action{"Shift F", ShellAction{"failing_command"}};
    auto res = executor.execute(action);
    CHECK(res.is_ok());

    REQUIRE(notifier.results.size() == 1);
    CHECK(notifier.results[0] == false); // rc != 0
    CHECK(notifier.hide_count == 1);
}

TEST_CASE("ActionExecutor - SendKeys Symbolic") {
    MockCommandRunner cmd_runner;
    MockInputSource input_source;
    MockStatusNotifier notifier;
    KeyTable key_table;
    AsciiTranslator ascii_translator;

    ActionExecutor executor(cmd_runner, input_source, key_table, ascii_translator, &notifier);

    Action action{"Shift S", SendKeysAction{"Space Enter"}};
    auto res = executor.execute(action);
    CHECK(res.is_ok());

    REQUIRE(input_source.injected_keys().size() == 2);
    CHECK(input_source.injected_keys()[0].code == KPKEY_SPACE);
    CHECK(input_source.injected_keys()[1].code == KPKEY_ENTER);
}

TEST_CASE("ActionExecutor - SendKeys Quoted String") {
    MockCommandRunner cmd_runner;
    MockInputSource input_source;
    MockStatusNotifier notifier;
    KeyTable key_table;
    AsciiTranslator ascii_translator;

    ActionExecutor executor(cmd_runner, input_source, key_table, ascii_translator, &notifier);

    Action action{"Shift T", SendKeysAction{"\"Hi\""}};
    auto res = executor.execute(action);
    CHECK(res.is_ok());

    REQUIRE(input_source.injected_keys().size() == 2);
    CHECK(input_source.injected_keys()[0].code == KPKEY_H);
    CHECK(input_source.injected_keys()[0].mode == SendMode::Shift);
    CHECK(input_source.injected_keys()[1].code == KPKEY_I);
    CHECK(input_source.injected_keys()[1].mode == SendMode::Normal);
}

TEST_CASE("ActionExecutor - Screenshot") {
    MockCommandRunner cmd_runner;
    MockInputSource input_source;
    MockStatusNotifier notifier;
    KeyTable key_table;
    AsciiTranslator ascii_translator;

    ActionExecutor executor(cmd_runner, input_source, key_table, ascii_translator, &notifier);

    Action action{"[Screenshot]", ScreenshotAction{}};
    auto res = executor.execute(action);
    CHECK(res.is_ok());

    REQUIRE(notifier.status_messages.size() == 1);
    CHECK(notifier.status_messages[0] == "[Screenshot]");
    REQUIRE(notifier.results.size() == 1);
    CHECK(notifier.results[0] == true);
}
