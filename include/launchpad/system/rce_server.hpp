/**
 * @file rce_server.hpp
 * @brief UDP multicast Remote Command Entry (RCE) server.
 * @details Listens on a configured multicast group/port (default: 239.1.2.3:10000 or 224.0.0.1:4444)
 *          to allow remote trigger messages over WiFi or usbnet.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/hal/command_runner.hpp"
#include <string>
#include <memory>

namespace launchpad::system {

/**
 * @brief Network configuration for the UDP multicast RCE listener.
 */
struct RceConfig {
    bool auto_enable{false};                 ///< Whether listener starts enabled on boot.
    std::string enable_string;               ///< Passphrase enabling execution.
    std::string disable_string;              ///< Passphrase disabling execution.
    std::string multicast_group{"239.1.2.3"};///< Multicast IP group to join.
    std::string interface_ip{"0.0.0.0"};     ///< Local interface IP address.
    uint16_t port{10000};                    ///< UDP port number.
};

/**
 * @brief Operational context and dependencies for the RCE server.
 */
struct RceContext {
    RceConfig config;                     ///< Server configuration settings.
    hal::ICommandRunner& command_runner;  ///< Command runner executing received payloads.
};

struct RceWorker;

/**
 * @brief Background UDP listener accepting remote trigger commands.
 */
class RceServer {
public:
    /**
     * @brief Constructs an RceServer with the given configuration and command runner.
     * @param[in] config RceConfig parameters.
     * @param[in] runner ICommandRunner for dispatching authorized commands.
     */
    RceServer(RceConfig config, hal::ICommandRunner& runner);

    /**
     * @brief Destructor stopping the listener thread and closing sockets.
     */
    ~RceServer();

    RceServer(const RceServer&) = delete;
    RceServer& operator=(const RceServer&) = delete;

    /**
     * @brief Starts the background UDP multicast listener worker thread.
     * @return Result<void> indicating socket bind and thread start success.
     */
    [[nodiscard]] core::Result<void> start();

    /**
     * @brief Stops the background worker thread and closes network sockets.
     */
    void stop();

    /**
     * @brief Checks if the background worker thread is currently running.
     * @return True if running, false otherwise.
     */
    [[nodiscard]] bool is_running() const noexcept;

    /**
     * @brief Evaluates an incoming UDP message and updates server authorization state.
     * @param[in] message Payload string received over network.
     * @param[in,out] is_enabled Flag indicating whether command execution is currently permitted.
     */
    void process_message(std::string_view message, bool& is_enabled);

private:
    RceContext context_;
    std::unique_ptr<RceWorker> worker_;
};

} // namespace launchpad::system
