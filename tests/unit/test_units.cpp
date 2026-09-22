#include "../test_framework.hpp"
#include "launchpad/core/result.hpp"
#include "launchpad/core/units.hpp"
#include "launchpad/core/file_descriptor.hpp"

using namespace launchpad::core;

TEST_CASE("Core - Result<T, Error>") {
    SECTION("Result holding value") {
        Result<int> res(42);
        REQUIRE(res.is_ok());
        CHECK_FALSE(res.is_err());
        CHECK(res.value() == 42);
        CHECK(res.value_or(10) == 42);
    }

    SECTION("Result holding error") {
        Result<int> res(Error("computation failed"));
        REQUIRE(res.is_err());
        CHECK_FALSE(res.is_ok());
        CHECK(res.error().message == "computation failed");
        CHECK(res.value_or(10) == 10);
    }

    SECTION("Result<void> handling") {
        Result<void> ok_res;
        CHECK(ok_res.is_ok());

        Result<void> err_res(Error("void error"));
        CHECK(err_res.is_err());
        CHECK(err_res.error().message == "void error");
    }
}

TEST_CASE("Core - Units & Typesafe Values") {
    SECTION("Milliseconds comparisons") {
        Milliseconds m1(500);
        Milliseconds m2(700);
        CHECK(m1 < m2);
        CHECK(m1 == Milliseconds(500));
    }

    SECTION("KeySequence operations") {
        KeySequence seq;
        CHECK(seq.empty());
        seq.push(10);
        seq.push(20);
        CHECK(seq.size() == 2);
        CHECK_FALSE(seq.empty());

        KeySequence seq2({10, 20});
        CHECK(seq == seq2);
    }
}

TEST_CASE("Core - ScopedFileDescriptor RAII") {
    SECTION("Default constructor is invalid") {
        ScopedFileDescriptor fd;
        CHECK_FALSE(fd.is_valid());
        CHECK(fd.get() == -1);
    }

    SECTION("Release detaches descriptor") {
        ScopedFileDescriptor fd(99);
        CHECK(fd.is_valid());
        int raw = fd.release();
        CHECK(raw == 99);
        CHECK_FALSE(fd.is_valid());
    }
}
