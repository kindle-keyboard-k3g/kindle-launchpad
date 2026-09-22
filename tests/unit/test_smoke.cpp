#include "../test_framework.hpp"

TEST_CASE("Smoke Test - Harness Integrity") {
    SECTION("Basic assertions work") {
        REQUIRE(1 + 1 == 2);
        CHECK(true);
        CHECK_FALSE(false);
    }
}
