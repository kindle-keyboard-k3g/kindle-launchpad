/**
 * @file logger.hpp
 * @brief Diagnostic logging facility for Kindle Launchpad.
 * @details Directs log messages to stderr when attached to a TTY, or to launchpad.log
 *          when running daemonized in the background.
 */

#pragma once

#include <string>
#include <string_view>
#include <cstdio>

namespace launchpad::core {

/**
 * @brief Severity level for diagnostic log entries.
 */
enum class LogLevel {
    Debug,   ///< Detailed diagnostic information for debugging and development.
    Info,    ///< Normal operational milestones (initialization, config reload).
    Warning, ///< Non-critical issues or recoverable errors.
    Error    ///< Severe failures preventing requested actions from completing.
};

/**
 * @brief Thread-safe diagnostic logging singleton.
 * @details Supports output to standard error streams or persistent log files.
 */
class Logger {
public:
    /**
     * @brief Accesses the global Logger singleton instance.
     * @return Reference to the singleton Logger.
     */
    static Logger& instance();

    /**
     * @brief Initializes the logging destination.
     * @param[in] log_file_path Path to the log file (e.g. launchpad.log).
     * @param[in] log_to_file If true, writes to file; if false, writes to stderr.
     */
    void init(const std::string& log_file_path, bool log_to_file);

    /**
     * @brief Closes the currently active log file descriptor.
     */
    void close();

    /**
     * @brief Emits a log entry with the specified severity level.
     * @param[in] level Severity level of the log entry.
     * @param[in] message Log message text.
     */
    void log(LogLevel level, std::string_view message);

    /**
     * @brief Emits a Debug-level log message.
     * @param[in] message Diagnostic debug message.
     */
    void debug(std::string_view message);

    /**
     * @brief Emits an Info-level log message.
     * @param[in] message Informational status message.
     */
    void info(std::string_view message);

    /**
     * @brief Emits a Warning-level log message.
     * @param[in] message Warning message.
     */
    void warning(std::string_view message);

    /**
     * @brief Emits an Error-level log message.
     * @param[in] message Error description.
     */
    void error(std::string_view message);

private:
    Logger() = default;
    ~Logger();

    FILE* file_{nullptr};     ///< Open file handle for file logging.
    bool log_to_file_{false}; ///< Whether file logging is currently active.
};

} // namespace launchpad::core
