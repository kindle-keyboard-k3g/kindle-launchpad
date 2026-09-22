#include "launchpad/core/logger.hpp"
#include <iostream>

int main() {
    launchpad::core::Logger::instance().info("Kindle Launchpad Modern C++20 initialized");
    return 0;
}
