/**
 * @file signal_handler.hpp
 * @brief Async-signal-safe POSIX signal handling dispatcher.
 * @details Traps SIGINT, SIGTERM, and SIGHUP, modifying std::atomic flags to safely
 *          request graceful shutdown or dynamic INI configuration reloads.
 */

#pragma once

#include <atomic>
#include <csignal>

namespace launchpad::system {

/**
 * @brief Thread-safe, async-signal-safe POSIX signal dispatcher.
 */
class SignalHandler {
public:
    /**
     * @brief Registers signal handlers for SIGINT, SIGTERM, and SIGHUP using sigaction().
     */
    static void install();

    /**
     * @brief Programmatically signals the application event loop to terminate.
     */
    static void request_shutdown() noexcept;

    /**
     * @brief Programmatically requests an INI configuration reload.
     */
    static void request_reload() noexcept;

    /**
     * @brief Resets the reload requested flag after processing.
     */
    static void clear_reload() noexcept;

    /**
     * @brief Checks if a termination signal (SIGINT, SIGTERM) was received.
     * @return True if shutdown is requested, false otherwise.
     */
    [[nodiscard]] static bool is_shutdown_requested() noexcept;

    /**
     * @brief Checks if a reload signal (SIGHUP) was received.
     * @return True if reload is pending, false otherwise.
     */
    [[nodiscard]] static bool is_reload_requested() noexcept;

private:
    static inline std::atomic<bool> shutdown_requested_{false};
    static inline std::atomic<bool> reload_requested_{false};

    static void handle_signal(int signal_number);
};

} // namespace launchpad::system
