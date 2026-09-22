#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/domain/action.hpp"
#include "launchpad/domain/key_definition.hpp"
#include "launchpad/domain/ascii_translator.hpp"
#include "launchpad/hal/command_runner.hpp"
#include "launchpad/hal/key_injector.hpp"
#include <string_view>

namespace launchpad::domain {

class IStatusNotifier {
public:
    virtual ~IStatusNotifier() = default;

    virtual void show_status(std::string_view message) = 0;
    virtual void show_result(bool success) = 0;
    virtual void hide_status() = 0;
};

struct ExecutorServices {
    hal::ICommandRunner& command_runner;
    hal::IKeyInjector& key_injector;
    IStatusNotifier* status_notifier{nullptr};
};

struct ExecutorContext {
    const KeyTable& key_table;
    const AsciiTranslator& ascii_translator;
};

class ActionExecutor {
public:
    ActionExecutor(hal::ICommandRunner& runner,
                   hal::IKeyInjector& injector,
                   const KeyTable& table,
                   const AsciiTranslator& translator,
                   IStatusNotifier* notifier = nullptr) noexcept;

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
