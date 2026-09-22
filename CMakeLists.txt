cmake_minimum_required(VERSION 3.20)
project(launchpad LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

# Option to build unit tests (enabled by default for host, disabled when cross-compiling)
if(CMAKE_CROSSCOMPILING)
    option(BUILD_TESTING "Build test suite" OFF)
else()
    option(BUILD_TESTING "Build test suite" ON)
endif()

# Core Launchpad C++ library
file(GLOB_RECURSE LAUNCHPAD_CORE_SOURCES
    "src/core/*.cpp"
    "src/config/*.cpp"
    "src/domain/*.cpp"
    "src/hal/*.cpp"
    "src/ui/*.cpp"
    "src/system/*.cpp"
)

add_library(launchpad_core STATIC ${LAUNCHPAD_CORE_SOURCES})
target_include_directories(launchpad_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
set_project_warnings(launchpad_core)
target_link_libraries(launchpad_core PUBLIC pthread)

# Main Launchpad executable
add_executable(launchpad src/main.cpp)
target_link_libraries(launchpad PRIVATE launchpad_core)
set_project_warnings(launchpad)

# Testing
if(BUILD_TESTING)
    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES
        "tests/unit/*.cpp"
        "tests/integration/*.cpp"
        "tests/mocks/*.cpp"
    )

    add_executable(unit_tests
        tests/test_main.cpp
        ${TEST_SOURCES}
    )
    target_link_libraries(unit_tests PRIVATE launchpad_core)
    target_include_directories(unit_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    set_project_warnings(unit_tests)

    add_test(NAME unit_tests COMMAND unit_tests)
endif()

# Packaging
include(cmake/KindlePackaging.cmake)

