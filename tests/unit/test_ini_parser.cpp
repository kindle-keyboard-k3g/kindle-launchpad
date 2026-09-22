#include "../test_framework.hpp"
#include "launchpad/config/ini_parser.hpp"

using namespace launchpad::config;

TEST_CASE("Config - IniParser String Parsing") {
    SECTION("Basic sections and keys") {
        std::string sample = R"(
; Global comment
[Settings]
Introducer = Shift
HotInterval = 700
InterKeyDelay = 50

# Actions comment
[Actions]
U N = !/mnt/us/usbnet/bin/usbnet toggle
G H = "http://github.com"
)";

        auto res = IniParser::parse_string(sample);
        REQUIRE(res.is_ok());
        const auto& doc = res.value();

        CHECK(doc.has_section("Settings"));
        CHECK(doc.has_section("settings")); // Case-insensitive
        CHECK(doc.has_section("Actions"));

        CHECK(doc.get_string("Settings", "Introducer") == "Shift");
        CHECK(doc.get_string("Settings", "introducer") == "Shift"); // Key case-insensitive
        CHECK(doc.get_int("Settings", "HotInterval") == 700);
        CHECK(doc.get_int("Settings", "InterKeyDelay") == 50);

        auto actions = doc.get_section_entries("Actions");
        REQUIRE(actions.size() == 2);
        CHECK(actions[0].key == "U N");
        CHECK(actions[0].value == "!/mnt/us/usbnet/bin/usbnet toggle");
        CHECK(actions[1].key == "G H");
        CHECK(actions[1].value == "\"http://github.com\"");
    }

    SECTION("Comments and blank lines are ignored") {
        std::string sample = "\n\n; comment 1\n# comment 2\n[Test]\nfoo = bar\n\n";
        auto res = IniParser::parse_string(sample);
        REQUIRE(res.is_ok());
        const auto& doc = res.value();
        CHECK(doc.get_string("Test", "foo") == "bar");
    }

    SECTION("Default values for missing keys") {
        std::string sample = "[Sec]\nk1 = v1\n";
        auto res = IniParser::parse_string(sample);
        REQUIRE(res.is_ok());
        const auto& doc = res.value();
        CHECK(doc.get_int("Sec", "nonexistent", 42) == 42);
        CHECK_FALSE(doc.get_string("Sec", "nonexistent").has_value());
    }
}
