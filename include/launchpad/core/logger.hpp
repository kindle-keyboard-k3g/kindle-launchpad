#pragma once

#include <string>
#include <string_view>
#include <cstdio>

namespace launchpad::core {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger& instance();

    void init(const std::string& log_file_path, bool log_to_file);
    void close();

    void log(LogLevel level, std::string_view message);
    void debug(std::string_view message);
    void info(std::string_view message);
    void warning(std::string_view message);
    void error(std::string_view message);

private:
    Logger() = default;
    ~Logger();

    FILE* file_{nullptr};
    bool log_to_file_{false};
};

} // namespace launchpad::core
