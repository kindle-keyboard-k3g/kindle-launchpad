#include "launchpad/core/logger.hpp"
#include <iostream>
#include <chrono>
#include <ctime>

namespace launchpad::core {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

Logger::~Logger() {
    close();
}

void Logger::init(const std::string& log_file_path, bool log_to_file) {
    close();
    log_to_file_ = log_to_file;
    if (log_to_file && !log_file_path.empty()) {
        file_ = std::fopen(log_file_path.c_str(), "a");
    }
}

void Logger::close() {
    if (file_) {
        std::fclose(file_);
        file_ = nullptr;
    }
}

void Logger::log(LogLevel level, std::string_view message) {
    const char* tag = "INFO";
    switch (level) {
        case LogLevel::Debug: tag = "DEBUG"; break;
        case LogLevel::Info: tag = "INFO"; break;
        case LogLevel::Warning: tag = "WARN"; break;
        case LogLevel::Error: tag = "ERROR"; break;
    }

    if (file_) {
        std::fprintf(file_, "[%s] %.*s\n", tag, static_cast<int>(message.size()), message.data());
        std::fflush(file_);
    } else {
        std::cerr << "[" << tag << "] " << message << "\n";
    }
}

void Logger::debug(std::string_view message) {
    log(LogLevel::Debug, message);
}

void Logger::info(std::string_view message) {
    log(LogLevel::Info, message);
}

void Logger::warning(std::string_view message) {
    log(LogLevel::Warning, message);
}

void Logger::error(std::string_view message) {
    log(LogLevel::Error, message);
}

} // namespace launchpad::core
