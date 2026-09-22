#include "launchpad/domain/action.hpp"
#include "launchpad/domain/action_registry.hpp"
#include "launchpad/domain/key_definition.hpp"
#include "test_framework.hpp"

using namespace launchpad::core;
using namespace launchpad::domain;
using namespace launchpad::domain::key_codes;

TEST_CASE("Action and ActionParser") {
    KeyTable key_table;

    SECTION("Parses ShellAction (! prefix)") {
        auto action = ActionParser::parse_command("!reboot", "Shift R", "");
        CHECK(action.display_name == "Shift R");
        REQUIRE(std::holds_alternative<ShellAction>(action.payload));
        CHECK(std::get<ShellAction>(action.payload).command == "reboot");
    }

    SECTION("Parses ScriptAction (@ prefix)") {
        auto action = ActionParser::parse_command("@test.sh", "Shift T", "/mnt/us/launchpad/scripts");
        REQUIRE(std::holds_alternative<ScriptAction>(action.payload));
        CHECK(std::get<ScriptAction>(action.payload).script_path == "/mnt/us/launchpad/scripts/test.sh");
    }

    SECTION("Parses ScriptAction with absolute path") {
        auto action = ActionParser::parse_command("@/bin/custom.sh", "Shift C", "/mnt/us/launchpad/scripts");
        REQUIRE(std::holds_alternative<ScriptAction>(action.payload));
        CHECK(std::get<ScriptAction>(action.payload).script_path == "/bin/custom.sh");
    }

    SECTION("Parses KindleKeysAction (# prefix)") {
        auto action = ActionParser::parse_command("#DEL", "Shift D", "");
        REQUIRE(std::holds_alternative<KindleKeysAction>(action.payload));
        CHECK(std::get<KindleKeysAction>(action.payload).keys == "DEL");
    }

    SECTION("Parses SendKeysAction (default or escaped)") {
        auto action1 = ActionParser::parse_command("Hello World", "Shift H", "");
        REQUIRE(std::holds_alternative<SendKeysAction>(action1.payload));
        CHECK(std::get<SendKeysAction>(action1.payload).keys == "Hello World");

        auto action2 = ActionParser::parse_command("\\!NotShell", "Shift N", "");
        REQUIRE(std::holds_alternative<SendKeysAction>(action2.payload));
        CHECK(std::get<SendKeysAction>(action2.payload).keys == "!NotShell");
    }

    SECTION("Parses KeySequence from string") {
        auto seq = ActionParser::parse_sequence("Shift Space", key_table);
        REQUIRE(seq.has_value());
        REQUIRE(seq->size() == 2);
        CHECK(seq->codes[0] == KPKEY_SHIFT);
        CHECK(seq->codes[1] == KPKEY_SPACE);
    }

    SECTION("Rejects invalid key names in sequence") {
        auto seq = ActionParser::parse_sequence("Shift NonExistentKey", key_table);
        CHECK_FALSE(seq.has_value());
    }
}

TEST_CASE("ActionRegistry collection") {
    ActionRegistry registry;
    KeySequence seq_space({KPKEY_SHIFT, KPKEY_SPACE});
    Action action_space{"Shift Space", ShellAction{"sync"}};

    SECTION("Registers and finds action by sequence") {
        registry.register_action(seq_space, action_space);
        auto found = registry.find_action(seq_space);
        REQUIRE(found.has_value());
        CHECK(found->display_name == "Shift Space");
        REQUIRE(std::holds_alternative<ShellAction>(found->payload));
    }

    SECTION("Returns nullopt for unknown sequence") {
        KeySequence unknown({KPKEY_1, KPKEY_2});
        auto found = registry.find_action(unknown);
        CHECK_FALSE(found.has_value());
    }

    SECTION("Recognizes built-in Screenshot sequence (double shift)") {
        KeySequence double_shift({KPKEY_SHIFT, KPKEY_SHIFT});
        auto found = registry.find_action(double_shift);
        REQUIRE(found.has_value());
        CHECK(found->display_name == "[Screenshot]");
        REQUIRE(std::holds_alternative<ScreenshotAction>(found->payload));
    }

    SECTION("Does not overwrite on duplicate registration") {
        registry.register_action(seq_space, action_space);
        Action duplicate{"Shift Space", ShellAction{"different"}};
        registry.register_action(seq_space, duplicate);
        auto found = registry.find_action(seq_space);
        REQUIRE(found.has_value());
        CHECK(std::get<ShellAction>(found->payload).command == "sync");
    }
}
