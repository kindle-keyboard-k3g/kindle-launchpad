#include "launchpad/system/application.hpp"
#include "launchpad/system/signal_handler.hpp"
#include "launchpad/system/rce_server.hpp"
#include "launchpad/config/config_scanner.hpp"
#include "launchpad/domain/action_registry.hpp"
#include "launchpad/domain/action_executor.hpp"
#include "launchpad/domain/sequence_matcher.hpp"
#include "launchpad/ui/status_bar.hpp"
#include <chrono>

namespace launchpad::system {

struct ApplicationContext {
    domain::KeyTable key_table{};
    domain::AsciiTranslator ascii_translator{};
    domain::ActionRegistry registry{};
    domain::SequenceMatcher matcher{domain::MatcherConfig{}, &key_table};
    ui::FontRenderer font{};
    std::unique_ptr<ui::StatusBar> status_bar;
    std::unique_ptr<domain::ActionExecutor> executor;
    std::unique_ptr<RceServer> rce_server;
    std::string config_directory;
    bool is_capturing{false};
    std::chrono::steady_clock::time_point last_key_time{};
    core::Milliseconds hot_interval{700};
};

Application::Application(ApplicationServices services, std::string config_directory)
    : services_(services),
      context_(std::make_unique<ApplicationContext>()) {
    context_->config_directory = std::move(config_directory);
    context_->status_bar = std::make_unique<ui::StatusBar>(services_.display, context_->font);
    context_->executor = std::make_unique<domain::ActionExecutor>(
        services_.runner,
        services_.injector,
        context_->key_table,
        context_->ascii_translator,
        context_->status_bar.get()
    );
    load_configuration();
}

Application::~Application() {
    if (context_->rce_server) {
        context_->rce_server->stop();
    }
}

void Application::reload_configuration() {
    SignalHandler::clear_reload();
    load_configuration();
    if (context_->status_bar) {
        context_->status_bar->show_status("Reloaded");
    }
}

void Application::load_configuration() {
    auto scanned = config::ConfigScanner::scan_directory(context_->config_directory);
    context_->registry.clear();
    for (const auto& entry : scanned.actions) {
        auto seq_opt = domain::ActionParser::parse_sequence(entry.key, context_->key_table);
        if (seq_opt) {
            auto action = domain::ActionParser::parse_command(
                entry.value,
                entry.key,
                scanned.settings.script_directory
            );
            context_->registry.register_action(*seq_opt, action);
        }
    }
    context_->hot_interval = scanned.settings.hot_interval;
    auto intro = context_->key_table.find_code(scanned.settings.introducer_key);
    auto trail = context_->key_table.find_code(scanned.settings.trailer_key);
    domain::MatcherConfig config{
        intro.value_or(domain::key_codes::KPKEY_SHIFT),
        trail.value_or(domain::key_codes::KPKEY_ENTER),
        &context_->key_table
    };
    context_->matcher = domain::SequenceMatcher(config, &context_->key_table);
}

void Application::handle_completed_match(const domain::MatcherResult& result) {
    if (context_->is_capturing) {
        services_.input.release();
        context_->is_capturing = false;
    }
    context_->status_bar->hide_status();
    auto action_opt = context_->registry.find_action(result.sequence);
    if (action_opt) {
        context_->executor->execute(*action_opt);
    }
}

void Application::process_key_event(const hal::InputKeyEvent& event) {
    auto result = context_->matcher.process_key_event(event.code, event.type);
    if (result.type == domain::MatcherActionType::StartCapture) {
        if (!context_->is_capturing) {
            services_.input.grab();
            context_->is_capturing = true;
        }
        context_->status_bar->show_status("^[...]");
        context_->last_key_time = std::chrono::steady_clock::now();
        return;
    }
    if (result.type == domain::MatcherActionType::KeyAppended) {
        context_->status_bar->show_status("^[...]");
        context_->last_key_time = std::chrono::steady_clock::now();
        return;
    }
    if (result.type == domain::MatcherActionType::SequenceComplete) {
        handle_completed_match(result);
        return;
    }
    if (result.type == domain::MatcherActionType::Cancelled) {
        if (context_->is_capturing) {
            services_.input.release();
            context_->is_capturing = false;
        }
        context_->status_bar->hide_status();
    }
}

void Application::check_timer() {
    if (!context_->is_capturing) return;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - context_->last_key_time);
    if (static_cast<uint32_t>(elapsed.count()) < context_->hot_interval.value) return;

    auto result = context_->matcher.on_timeout();
    if (result.type == domain::MatcherActionType::SequenceComplete) {
        handle_completed_match(result);
        return;
    }
    services_.input.release();
    context_->is_capturing = false;
    context_->status_bar->hide_status();
}

void Application::step() {
    auto events = services_.input.poll_events(core::Milliseconds{50});
    for (const auto& ev : events) {
        process_key_event(ev);
    }
    check_timer();
}

int Application::run() {
    while (!SignalHandler::is_shutdown_requested()) {
        if (SignalHandler::is_reload_requested()) {
            reload_configuration();
        }
        step();
    }
    return 0;
}

} // namespace launchpad::system
