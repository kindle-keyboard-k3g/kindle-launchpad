#include "launchpad/domain/key_definition.hpp"
#include "test_framework.hpp"

using namespace launchpad::core;
using namespace launchpad::domain;

TEST_CASE("KeyDefinition default mappings") {
    KeyTable table;

    SECTION("Resolves canonical key names to keycodes") {
        auto code_shift = table.find_code("Shift");
        REQUIRE(code_shift.has_value());
        CHECK(code_shift.value() == 42);

        auto code_enter = table.find_code("Enter");
        REQUIRE(code_enter.has_value());
        CHECK(code_enter.value() == 28);

        auto code_space = table.find_code("Space");
        REQUIRE(code_space.has_value());
        CHECK(code_space.value() == 57);
    }

    SECTION("Name lookup is case-insensitive") {
        auto code_lower = table.find_code("shift");
        REQUIRE(code_lower.has_value());
        CHECK(code_lower.value() == 42);

        auto code_upper = table.find_code("SPACE");
        REQUIRE(code_upper.has_value());
        CHECK(code_upper.value() == 57);
    }

    SECTION("Resolves keycode to name") {
        auto name_shift = table.find_name(42);
        REQUIRE(name_shift.has_value());
        CHECK(name_shift.value() == "Shift");

        auto name_enter = table.find_name(28);
        REQUIRE(name_enter.has_value());
        CHECK(name_enter.value() == "Enter");
    }

    SECTION("Maps Kindle 3 keycodes to canonical DXG substitutes") {
        // K3KEY_HOME (102) -> KPKEY_HOME (98)
        CHECK(table.get_substitute_code(102) == 98);

        // K3KEY_BACK (158) -> KPKEY_BACK (91)
        CHECK(table.get_substitute_code(158) == 91);

        // K3KEY_SYM (126) -> KPKEY_SYM (94)
        CHECK(table.get_substitute_code(126) == 94);

        // Regular key should substitute to itself
        CHECK(table.get_substitute_code(42) == 42);
    }

    SECTION("Allows registering custom key definitions") {
        table.register_key(200, "CustomKey");
        auto code = table.find_code("CustomKey");
        REQUIRE(code.has_value());
        CHECK(code.value() == 200);

        auto name = table.find_name(200);
        REQUIRE(name.has_value());
        CHECK(name.value() == "CustomKey");
    }
}
