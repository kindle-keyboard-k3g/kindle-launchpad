#pragma once

#include <atomic>
#include <csignal>

namespace launchpad::system {

class SignalHandler {
public:
    static void install();
    static void request_shutdown() noexcept;
    static void request_reload() noexcept;
    static void clear_reload() noexcept;

    [[nodiscard]] static bool is_shutdown_requested() noexcept;
    [[nodiscard]] static bool is_reload_requested() noexcept;

private:
    static inline std::atomic<bool> shutdown_requested_{false};
    static inline std::atomic<bool> reload_requested_{false};

    static void handle_signal(int signal_number);
};

} // namespace launchpad::system
