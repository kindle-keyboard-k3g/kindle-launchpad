#include "launchpad/system/signal_handler.hpp"

namespace launchpad::system {

void SignalHandler::handle_signal(int signal_number) {
    if (signal_number == SIGHUP) {
        reload_requested_.store(true, std::memory_order_relaxed);
        return;
    }
    if (signal_number == SIGINT || signal_number == SIGTERM) {
        shutdown_requested_.store(true, std::memory_order_relaxed);
        return;
    }
}

void SignalHandler::install() {
    shutdown_requested_.store(false, std::memory_order_relaxed);
    reload_requested_.store(false, std::memory_order_relaxed);

    struct sigaction action{};
    action.sa_handler = &SignalHandler::handle_signal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);
    sigaction(SIGHUP, &action, nullptr);
}

void SignalHandler::request_shutdown() noexcept {
    shutdown_requested_.store(true, std::memory_order_relaxed);
}

void SignalHandler::request_reload() noexcept {
    reload_requested_.store(true, std::memory_order_relaxed);
}

void SignalHandler::clear_reload() noexcept {
    reload_requested_.store(false, std::memory_order_relaxed);
}

bool SignalHandler::is_shutdown_requested() noexcept {
    return shutdown_requested_.load(std::memory_order_relaxed);
}

bool SignalHandler::is_reload_requested() noexcept {
    return reload_requested_.load(std::memory_order_relaxed);
}

} // namespace launchpad::system
