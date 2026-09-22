/**
 * @file mock_input_source.hpp
 * @brief In-memory mock input source and key injector for host testing.
 * @details Implements IInputSource and IKeyInjector test doubles allowing unit and
 *          integration tests to inject synthetic key sequences and verify simulated keystrokes.
 */

#pragma once

#include "launchpad/hal/input_source.hpp"
#include "launchpad/hal/key_injector.hpp"

namespace launchpad::hal {

/**
 * @brief Represents a synthetic key injected through the IKeyInjector interface in tests.
 */
struct InjectedKey {
    core::RawKeyCode code{0};                          ///< Raw scancode that was injected.
    domain::SendMode mode{domain::SendMode::Normal};   ///< Modifier mode used during injection.
    auto operator<=>(const InjectedKey&) const = default;
};

/**
 * @brief Internal state of the mock input event queue and grab status.
 */
struct MockInputState {
    bool grabbed{false};                        ///< Whether grab() is currently asserted.
    std::vector<InputKeyEvent> event_queue;     ///< Queued key events waiting to be polled.
};

/**
 * @brief Internal record of keys injected via inject_key() or inject_ascii_char().
 */
struct MockInjectionState {
    std::vector<InjectedKey> keys; ///< History of keys injected during the test.
};

/**
 * @brief Test double implementing IInputSource and IKeyInjector for host-based verification.
 */
class MockInputSource : public IInputSource, public IKeyInjector {
public:
    /**
     * @brief Simulates grabbing input devices, setting the grabbed flag to true.
     * @return Successful Result<void>.
     */
    core::Result<void> grab() override {
        input_state_.grabbed = true;
        return core::Result<void>();
    }

    /**
     * @brief Simulates releasing input devices, resetting the grabbed flag to false.
     * @return Successful Result<void>.
     */
    core::Result<void> release() override {
        input_state_.grabbed = false;
        return core::Result<void>();
    }

    /**
     * @brief Checks if input is currently grabbed.
     * @return True if grabbed, false otherwise.
     */
    [[nodiscard]] bool is_grabbed() const noexcept override {
        return input_state_.grabbed;
    }

    /**
     * @brief Drains all queued synthetic events and returns them.
     * @param[in] timeout Ignored in mock.
     * @return Vector of queued InputKeyEvent instances.
     */
    std::vector<InputKeyEvent> poll_events([[maybe_unused]] core::Milliseconds timeout) override {
        auto events = std::move(input_state_.event_queue);
        input_state_.event_queue.clear();
        return events;
    }

    /**
     * @brief Queues a synthetic key event to be returned by poll_events().
     * @param[in] event InputKeyEvent to enqueue.
     */
    void queue_event(InputKeyEvent event) {
        input_state_.event_queue.push_back(event);
    }

    /**
     * @brief Records an injected key scancode into the verification history.
     * @param[in] code Raw key code.
     * @param[in] mode Modifier mode.
     * @return Successful Result<void>.
     */
    core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) override {
        injection_state_.keys.push_back(InjectedKey{code, mode});
        return core::Result<void>();
    }

    /**
     * @brief Translates an ASCII character and records the resulting key into history.
     * @param[in] c ASCII character to emit.
     * @param[in] translator Translator instance.
     * @return Successful Result<void>.
     */
    core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) override {
        auto mapping = translator.translate(c);
        injection_state_.keys.push_back(InjectedKey{mapping.code, mapping.send_mode});
        return core::Result<void>();
    }

    /**
     * @brief Returns the list of all keys injected since construction or last clear().
     * @return Const reference to vector of InjectedKey records.
     */
    [[nodiscard]] const std::vector<InjectedKey>& injected_keys() const noexcept {
        return injection_state_.keys;
    }

    /**
     * @brief Resets queue, history, and grab state.
     */
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
