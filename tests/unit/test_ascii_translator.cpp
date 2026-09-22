#include "launchpad/domain/ascii_translator.hpp"
#include "launchpad/domain/key_definition.hpp"
#include "test_framework.hpp"

using namespace launchpad::core;
using namespace launchpad::domain;
using namespace launchpad::domain::key_codes;

TEST_CASE("AsciiTranslator char translation") {
    SECTION("Kindle DX translations") {
        AsciiTranslator translator(KindleModel::KindleDX);

        // Lowercase 'a' -> KPKEY_A, Normal
        auto map_a = translator.translate('a');
        CHECK(map_a.code == KPKEY_A);
        CHECK(map_a.send_mode == SendMode::Normal);
        CHECK_FALSE(map_a.symbol_navigation.has_value());

        // Uppercase 'A' -> KPKEY_A, Shift
        auto map_upper_a = translator.translate('A');
        CHECK(map_upper_a.code == KPKEY_A);
        CHECK(map_upper_a.send_mode == SendMode::Shift);

        // Digit '1' -> KPKEY_1, Normal
        auto map_1 = translator.translate('1');
        CHECK(map_1.code == KPKEY_1);
        CHECK(map_1.send_mode == SendMode::Normal);

        // Exclamation '!' -> KPKEY_1, Shift, with symbol nav
        auto map_excl = translator.translate('!');
        CHECK(map_excl.code == KPKEY_1);
        CHECK(map_excl.send_mode == SendMode::Shift);
        REQUIRE(map_excl.symbol_navigation.has_value());
        CHECK(map_excl.symbol_navigation->horizontal_steps == 1);
        CHECK(map_excl.symbol_navigation->vertical_steps == 0);
    }

    SECTION("Kindle 3 translations") {
        AsciiTranslator translator(KindleModel::Kindle3);

        // '!' on K3 has different symbol nav than DX
        auto map_excl = translator.translate('!');
        REQUIRE(map_excl.symbol_navigation.has_value());
        CHECK(map_excl.symbol_navigation->horizontal_steps == -1);
        CHECK(map_excl.symbol_navigation->vertical_steps == 1);
    }
}
