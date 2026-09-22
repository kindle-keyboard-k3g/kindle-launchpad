#include "launchpad/system/rce_server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <thread>
#include <atomic>

namespace launchpad::system {

struct RceWorker {
    std::thread thread;
    std::atomic<bool> running{false};
    int socket_fd{-1};
};

RceServer::RceServer(RceConfig config, hal::ICommandRunner& runner)
    : context_{std::move(config), runner},
      worker_(std::make_unique<RceWorker>()) {}

RceServer::~RceServer() {
    stop();
}

bool RceServer::is_running() const noexcept {
    return worker_->running.load();
}

void RceServer::process_message(std::string_view message, bool& is_enabled) {
    if (!is_enabled) {
        if (!context_.config.enable_string.empty() && message == context_.config.enable_string) {
            is_enabled = true;
        }
        return;
    }
    if (!context_.config.disable_string.empty() && message == context_.config.disable_string) {
        is_enabled = false;
        return;
    }
    if (message != context_.config.enable_string) {
        static_cast<void>(context_.command_runner.run_command(message));
    }
}

static int create_and_bind_socket(const RceConfig& config) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config.port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

core::Result<void> RceServer::start() {
    if (is_running()) return core::Result<void>();
    if (!context_.config.auto_enable && context_.config.enable_string.empty()) {
        return core::Result<void>();
    }
    int fd = create_and_bind_socket(context_.config);
    if (fd < 0) return core::Result<void>(core::Error("Failed to bind RCE socket"));

    worker_->socket_fd = fd;
    worker_->running.store(true);
    worker_->thread = std::thread([this]() {
        bool enabled = context_.config.auto_enable;
        char buffer[1024];
        while (worker_->running.load()) {
            pollfd pfd{worker_->socket_fd, POLLIN, 0};
            if (poll(&pfd, 1, 200) <= 0) continue;
            ssize_t bytes = recv(worker_->socket_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) continue;
            buffer[bytes] = '\0';
            process_message(std::string_view(buffer, static_cast<size_t>(bytes)), enabled);
        }
    });
    return core::Result<void>();
}

void RceServer::stop() {
    if (!is_running()) return;
    worker_->running.store(false);
    if (worker_->thread.joinable()) {
        worker_->thread.join();
    }
    if (worker_->socket_fd >= 0) {
        close(worker_->socket_fd);
        worker_->socket_fd = -1;
    }
}

} // namespace launchpad::system
