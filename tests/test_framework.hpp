#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <cstdlib>

namespace test_framework {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestCase>& get_registry() {
    static std::vector<TestCase> registry;
    return registry;
}

inline int& get_failed_count() {
    static int failed = 0;
    return failed;
}

inline int& get_passed_count() {
    static int passed = 0;
    return passed;
}

inline std::string& current_section() {
    static std::string section;
    return section;
}

struct AutoReg {
    AutoReg(const std::string& name, std::function<void()> func) {
        get_registry().push_back({name, std::move(func)});
    }
};

inline void record_failure(const char* file, int line, const char* expr, const std::string& message = "") {
    get_failed_count()++;
    std::cerr << "\033[1;31m[FAILED]\033[0m " << file << ":" << line << " in [" << current_section() << "]: " << expr;
    if (!message.empty()) {
        std::cerr << " (" << message << ")";
    }
    std::cerr << "\n";
}

inline void record_success() {
    get_passed_count()++;
}

struct SectionScope {
    std::string previous;
    explicit SectionScope(const std::string& name) : previous(current_section()) {
        current_section() = name;
    }
    ~SectionScope() {
        current_section() = previous;
    }
};

inline int run_all() {
    auto& tests = get_registry();
    std::cout << "\033[1;34m=== Running " << tests.size() << " test case(s) ===\033[0m\n";
    int test_failures = 0;
    for (const auto& test : tests) {
        current_section() = test.name;
        int failed_before = get_failed_count();
        try {
            test.func();
        } catch (const std::exception& e) {
            record_failure(__FILE__, __LINE__, "exception thrown", e.what());
        } catch (...) {
            record_failure(__FILE__, __LINE__, "unknown exception thrown");
        }
        if (get_failed_count() > failed_before) {
            test_failures++;
            std::cout << "\033[1;31m ✗ " << test.name << "\033[0m\n";
        } else {
            std::cout << "\033[1;32m ✓ " << test.name << "\033[0m\n";
        }
    }
    std::cout << "----------------------------------------\n";
    std::cout << "Results: " << get_passed_count() << " checks passed, "
              << get_failed_count() << " failed across " << tests.size() << " test(s).\n";
    return (get_failed_count() == 0) ? 0 : 1;
}

} // namespace test_framework

#define TEST_CONCAT_IMPL(a, b) a##b
#define TEST_CONCAT(a, b) TEST_CONCAT_IMPL(a, b)

#define TEST_CASE(name) \
    static void TEST_CONCAT(test_func_, __LINE__)(); \
    static ::test_framework::AutoReg TEST_CONCAT(reg_, __LINE__)(name, TEST_CONCAT(test_func_, __LINE__)); \
    static void TEST_CONCAT(test_func_, __LINE__)()

#define SECTION(name) \
    if (::test_framework::SectionScope TEST_CONCAT(section_scope_, __LINE__){name}; true)

#define REQUIRE(expr) \
    do { \
        if (expr) { \
            ::test_framework::record_success(); \
        } else { \
            ::test_framework::record_failure(__FILE__, __LINE__, #expr); \
            return; \
        } \
    } while (false)

#define CHECK(expr) \
    do { \
        if (expr) { \
            ::test_framework::record_success(); \
        } else { \
            ::test_framework::record_failure(__FILE__, __LINE__, #expr); \
        } \
    } while (false)

#define REQUIRE_FALSE(expr) REQUIRE(!(expr))
#define CHECK_FALSE(expr) CHECK(!(expr))
