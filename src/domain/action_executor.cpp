#include "launchpad/domain/action_executor.hpp"
#include <sstream>

namespace launchpad::domain {

ActionExecutor::ActionExecutor(hal::ICommandRunner& runner,
                               hal::IKeyInjector& injector,
                               const KeyTable& table,
                               const AsciiTranslator& translator,
                               IStatusNotifier* notifier) noexcept
    : services_{runner, injector, notifier},
      context_{table, translator} {}

void ActionExecutor::send_quoted_string(std::string_view str) {
    for (char c : str) {
        static_cast<void>(services_.key_injector.inject_ascii_char(c, context_.ascii_translator));
    }
}

void ActionExecutor::send_token(std::string_view token) {
    if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
        send_quoted_string(token.substr(1, token.size() - 2));
        return;
    }
    auto code = context_.key_table.find_code(token);
    if (code.has_value()) {
        static_cast<void>(services_.key_injector.inject_key(*code, SendMode::Normal));
        return;
    }
    send_quoted_string(token);
}

void ActionExecutor::execute_shell(const ShellAction& shell, std::string_view display_name) {
    if (services_.status_notifier != nullptr) {
        services_.status_notifier->show_status("^[" + std::string(display_name) + "]");
    }
    int rc = services_.command_runner.run_command(shell.command);
    if (services_.status_notifier != nullptr) {
        services_.status_notifier->show_result(rc == 0);
        services_.status_notifier->hide_status();
    }
}

void ActionExecutor::execute_send_keys(const SendKeysAction& keys) {
    std::istringstream stream{keys.keys};
    std::string token;
    while (stream >> token) {
        send_token(token);
    }
    if (services_.status_notifier != nullptr) {
        services_.status_notifier->hide_status();
    }
}

void ActionExecutor::execute_screenshot(std::string_view display_name) {
    if (services_.status_notifier != nullptr) {
        services_.status_notifier->show_status(display_name);
        services_.status_notifier->show_result(true);
        services_.status_notifier->hide_status();
    }
}

core::Result<void> ActionExecutor::execute(const Action& action) {
    if (std::holds_alternative<ShellAction>(action.payload)) {
        execute_shell(std::get<ShellAction>(action.payload), action.display_name);
        return core::Result<void>();
    }
    if (std::holds_alternative<SendKeysAction>(action.payload)) {
        execute_send_keys(std::get<SendKeysAction>(action.payload));
        return core::Result<void>();
    }
    if (std::holds_alternative<ScreenshotAction>(action.payload)) {
        execute_screenshot(action.display_name);
        return core::Result<void>();
    }
    return core::Result<void>();
}

} // namespace launchpad::domain
