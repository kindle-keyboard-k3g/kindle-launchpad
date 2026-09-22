#pragma once

#include "launchpad/hal/input_source.hpp"
#include "launchpad/hal/key_injector.hpp"

namespace launchpad::hal {

struct InjectedKey {
    core::RawKeyCode code{0};
    domain::SendMode mode{domain::SendMode::Normal};
    auto operator<=>(const InjectedKey&) const = default;
};

struct MockInputState {
    bool grabbed{false};
    std::vector<InputKeyEvent> event_queue;
};

struct MockInjectionState {
    std::vector<InjectedKey> keys;
};

class MockInputSource : public IInputSource, public IKeyInjector {
public:
    core::Result<void> grab() override {
        input_state_.grabbed = true;
        return core::Result<void>();
    }

    core::Result<void> release() override {
        input_state_.grabbed = false;
        return core::Result<void>();
    }

    [[nodiscard]] bool is_grabbed() const noexcept override {
        return input_state_.grabbed;
    }

    std::vector<InputKeyEvent> poll_events([[maybe_unused]] core::Milliseconds timeout) override {
        auto events = std::move(input_state_.event_queue);
        input_state_.event_queue.clear();
        return events;
    }

    void queue_event(InputKeyEvent event) {
        input_state_.event_queue.push_back(event);
    }

    core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) override {
        injection_state_.keys.push_back(InjectedKey{code, mode});
        return core::Result<void>();
    }

    core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) override {
        auto mapping = translator.translate(c);
        injection_state_.keys.push_back(InjectedKey{mapping.code, mapping.send_mode});
        return core::Result<void>();
    }

    [[nodiscard]] const std::vector<InjectedKey>& injected_keys() const noexcept {
        return injection_state_.keys;
    }

    void clear() noexcept {
        input_state_.event_queue.clear();
        injection_state_.keys.clear();
        input_state_.grabbed = false;
    }

private:
    MockInputState input_state_;
    MockInjectionState injection_state_;
};

} // namespace launchpad::hal
