#pragma once

#include "launchpad/core/units.hpp"
#include "launchpad/domain/key_definition.hpp"

namespace launchpad::domain {

enum class KeyEventType : uint8_t {
    Pressed = 1,
    Released = 0
};

enum class MatcherState : uint8_t {
    Idle,
    Capturing
};

enum class MatcherActionType : uint8_t {
    None,
    StartCapture,
    KeyAppended,
    SequenceComplete,
    Cancelled
};

struct MatcherResult {
    MatcherActionType type{MatcherActionType::None};
    core::KeySequence sequence{};
};

struct MatcherConfig {
    core::RawKeyCode introducer{key_codes::KPKEY_SHIFT};
    core::RawKeyCode trailer{key_codes::KPKEY_ENTER};
    const KeyTable* key_table{nullptr};
};

struct MatcherContext {
    MatcherState state{MatcherState::Idle};
    int keys_down{0};
    bool introducer_armed{false};
    core::KeySequence sequence{};
};

class SequenceMatcher {
public:
    explicit SequenceMatcher(MatcherConfig config, const KeyTable* table = nullptr) noexcept;

    [[nodiscard]] MatcherResult process_key_event(core::RawKeyCode code, KeyEventType type) noexcept;
    [[nodiscard]] MatcherResult on_timeout() noexcept;
    [[nodiscard]] MatcherResult cancel() noexcept;

    [[nodiscard]] MatcherState state() const noexcept;
    [[nodiscard]] const core::KeySequence& current_sequence() const noexcept;

private:
    [[nodiscard]] MatcherResult handle_idle_press(core::RawKeyCode code) noexcept;
    [[nodiscard]] MatcherResult handle_idle_release(core::RawKeyCode code) noexcept;
    [[nodiscard]] MatcherResult handle_capturing_press(core::RawKeyCode code) noexcept;
    [[nodiscard]] MatcherResult handle_capturing_release(core::RawKeyCode code) noexcept;
    [[nodiscard]] core::RawKeyCode substitute_code(core::RawKeyCode code) const noexcept;

    MatcherConfig config_;
    MatcherContext context_;
};

} // namespace launchpad::domain
