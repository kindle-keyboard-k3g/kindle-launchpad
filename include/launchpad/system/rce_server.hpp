#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/hal/command_runner.hpp"
#include <string>
#include <memory>

namespace launchpad::system {

struct RceConfig {
    bool auto_enable{false};
    std::string enable_string;
    std::string disable_string;
    std::string multicast_group{"239.1.2.3"};
    std::string interface_ip{"0.0.0.0"};
    uint16_t port{10000};
};

struct RceContext {
    RceConfig config;
    hal::ICommandRunner& command_runner;
};

struct RceWorker;

class RceServer {
public:
    RceServer(RceConfig config, hal::ICommandRunner& runner);
    ~RceServer();

    RceServer(const RceServer&) = delete;
    RceServer& operator=(const RceServer&) = delete;

    [[nodiscard]] core::Result<void> start();
    void stop();
    [[nodiscard]] bool is_running() const noexcept;

    void process_message(std::string_view message, bool& is_enabled);

private:
    RceContext context_;
    std::unique_ptr<RceWorker> worker_;
};

} // namespace launchpad::system
