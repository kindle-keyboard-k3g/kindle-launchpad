/**
 * @file action_executor.hpp
 * @brief Dispatcher and executor for domain actions across injected HAL services.
 * @details Unpacks ActionPayload variants and routes execution to ICommandRunner,
 *          IKeyInjector, or the internal screenshot exporter, updating status bar feedback.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/domain/action.hpp"
#include "launchpad/domain/key_definition.hpp"
#include "launchpad/domain/ascii_translator.hpp"
#include "launchpad/hal/command_runner.hpp"
#include "launchpad/hal/key_injector.hpp"
#include <string_view>

namespace launchpad::domain {

/**
 * @brief Observer interface for receiving visual status notifications during action execution.
 */
class IStatusNotifier {
public:
    virtual ~IStatusNotifier() = default;

    /**
     * @brief Displays an informational text message on the status bar.
     * @param[in] message Text to render (e.g. "^[...]" or action name).
     */
    virtual void show_status(std::string_view message) = 0;

    /**
     * @brief Displays an outcome indicator ("Success!" or "Failure.").
     * @param[in] success True if operation succeeded, false if error occurred.
     */
    virtual void show_result(bool success) = 0;

    /**
     * @brief Clears and hides the active status notification.
     */
    virtual void hide_status() = 0;
};

/**
 * @brief Aggregate of HAL dependencies required for executing actions.
 */
struct ExecutorServices {
    hal::ICommandRunner& command_runner;  ///< Service executing system shell commands and scripts.
    hal::IKeyInjector& key_injector;      ///< Service simulating physical keypresses.
    IStatusNotifier* status_notifier{nullptr}; ///< Optional observer for status notifications.
};

/**
 * @brief Aggregate of domain reference tables used during action execution.
 */
struct ExecutorContext {
    const KeyTable& key_table;                ///< Key definition lookup table.
    const AsciiTranslator& ascii_translator;  ///< ASCII character to scancode translator.
};

/**
 * @brief Core domain dispatcher executing matched actions using injected services.
 */
class ActionExecutor {
public:
    /**
     * @brief Constructs an ActionExecutor with injected dependencies.
     * @param[in] runner Command runner service.
     * @param[in] injector Key injector service.
     * @param[in] table Reference to KeyTable.
     * @param[in] translator Reference to AsciiTranslator.
     * @param[in] notifier Optional pointer to IStatusNotifier.
     */
    ActionExecutor(hal::ICommandRunner& runner,
                   hal::IKeyInjector& injector,
                   const KeyTable& table,
                   const AsciiTranslator& translator,
                   IStatusNotifier* notifier = nullptr) noexcept;

    /**
     * @brief Executes the specified Action entity.
     * @param[in] action Action containing payload and display name.
     * @return Result<void> indicating success or execution failure.
     */
    core::Result<void> execute(const Action& action);

private:
    void execute_shell(const ShellAction& shell, std::string_view display_name);
    void execute_send_keys(const SendKeysAction& keys);
    void execute_screenshot(std::string_view display_name);
    void send_token(std::string_view token);
    void send_quoted_string(std::string_view str);

    ExecutorServices services_;
    ExecutorContext context_;
};

} // namespace launchpad::domain
