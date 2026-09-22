/**
 * @file application.hpp
 * @brief Top-level application orchestrator and event loop composition root.
 * @details Couples HAL drivers, Domain state machines, Config parsers, and UI components
 *          into a cohesive polling event loop responding to hardware keys and signals.
 */

#pragma once

#include "launchpad/hal/input_source.hpp"
#include "launchpad/hal/display.hpp"
#include "launchpad/hal/command_runner.hpp"
#include "launchpad/hal/key_injector.hpp"
#include "launchpad/core/result.hpp"
#include <string>
#include <memory>

namespace launchpad::system {

/**
 * @brief Dependency container aggregating all required hardware abstraction services.
 */
struct ApplicationServices {
    hal::IInputSource& input;     ///< Hardware key event input source.
    hal::IDisplay& display;       ///< E-ink display framebuffer and update interface.
    hal::ICommandRunner& runner;  ///< Process execution service.
    hal::IKeyInjector& injector;  ///< Virtual keystroke injection service.
};

struct ApplicationContext;

/**
 * @brief Central daemon application orchestrating polling, sequence matching, and execution.
 */
class Application {
public:
    /**
     * @brief Constructs an Application with injected HAL services and configuration path.
     * @param[in] services Aggregated HAL dependencies.
     * @param[in] config_directory Directory to scan for INI files.
     */
    Application(ApplicationServices services, std::string config_directory);

    /**
     * @brief Destructor releasing resources and restoring hardware states.
     */
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * @brief Enters the main blocking event loop, terminating on shutdown signals.
     * @return Exit status code (0 on normal termination).
     */
    int run();

    /**
     * @brief Executes a single iteration of the polling and state processing loop.
     * @note Exposed for deterministic step-by-step unit and integration testing.
     */
    void step();

    /**
     * @brief Re-scans the configuration directory and refreshes action bindings dynamically.
     */
    void reload_configuration();

    /**
     * @brief Provides read-only access to internal application context for tests.
     * @return Const reference to ApplicationContext.
     */
    [[nodiscard]] const ApplicationContext& context() const noexcept {
        return *context_;
    }

private:
    ApplicationServices services_;
    std::unique_ptr<ApplicationContext> context_;

    void load_configuration();
    void process_key_event(const hal::InputKeyEvent& event);
    void check_timer();
    void handle_completed_match(const domain::MatcherResult& result);
};

} // namespace launchpad::system
