#include "launchpad/domain/sequence_matcher.hpp"

namespace launchpad::domain {

SequenceMatcher::SequenceMatcher(MatcherConfig config, const KeyTable* table) noexcept
    : config_(config) {
    if (table != nullptr) {
        config_.key_table = table;
    }
}

MatcherState SequenceMatcher::state() const noexcept {
    return context_.state;
}

const core::KeySequence& SequenceMatcher::current_sequence() const noexcept {
    return context_.sequence;
}

core::RawKeyCode SequenceMatcher::substitute_code(core::RawKeyCode code) const noexcept {
    if (config_.key_table == nullptr) {
        return code;
    }
    return config_.key_table->get_substitute_code(code);
}

MatcherResult SequenceMatcher::handle_idle_press(core::RawKeyCode code) noexcept {
    context_.keys_down++;
    if (context_.keys_down == 1 && code == config_.introducer) {
        context_.introducer_armed = true;
        return MatcherResult{MatcherActionType::None, {}};
    }
    context_.introducer_armed = false;
    return MatcherResult{MatcherActionType::None, {}};
}

MatcherResult SequenceMatcher::handle_idle_release(core::RawKeyCode code) noexcept {
    context_.keys_down = (context_.keys_down > 0) ? (context_.keys_down - 1) : 0;
    if (context_.keys_down == 0 && code == config_.introducer && context_.introducer_armed) {
        context_.introducer_armed = false;
        context_.state = MatcherState::Capturing;
        context_.sequence.clear();
        return MatcherResult{MatcherActionType::StartCapture, {}};
    }
    return MatcherResult{MatcherActionType::None, {}};
}

MatcherResult SequenceMatcher::handle_capturing_press(core::RawKeyCode code) noexcept {
    context_.keys_down++;
    if (code == config_.trailer) {
        auto finished = std::move(context_.sequence);
        context_.sequence.clear();
        context_.state = MatcherState::Idle;
        return MatcherResult{MatcherActionType::SequenceComplete, std::move(finished)};
    }
    core::RawKeyCode sub_code = substitute_code(code);
    context_.sequence.push(sub_code);
    return MatcherResult{MatcherActionType::KeyAppended, context_.sequence};
}

MatcherResult SequenceMatcher::handle_capturing_release([[maybe_unused]] core::RawKeyCode code) noexcept {
    context_.keys_down = (context_.keys_down > 0) ? (context_.keys_down - 1) : 0;
    return MatcherResult{MatcherActionType::None, {}};
}

MatcherResult SequenceMatcher::process_key_event(core::RawKeyCode code, KeyEventType type) noexcept {
    if (context_.state == MatcherState::Idle && type == KeyEventType::Pressed) {
        return handle_idle_press(code);
    }
    if (context_.state == MatcherState::Idle && type == KeyEventType::Released) {
        return handle_idle_release(code);
    }
    if (context_.state == MatcherState::Capturing && type == KeyEventType::Pressed) {
        return handle_capturing_press(code);
    }
    return handle_capturing_release(code);
}

MatcherResult SequenceMatcher::on_timeout() noexcept {
    if (context_.state != MatcherState::Capturing) {
        return MatcherResult{MatcherActionType::None, {}};
    }
    auto finished = std::move(context_.sequence);
    context_.sequence.clear();
    context_.state = MatcherState::Idle;
    return MatcherResult{MatcherActionType::SequenceComplete, std::move(finished)};
}

MatcherResult SequenceMatcher::cancel() noexcept {
    context_.sequence.clear();
    context_.state = MatcherState::Idle;
    context_.introducer_armed = false;
    context_.keys_down = 0;
    return MatcherResult{MatcherActionType::Cancelled, {}};
}

} // namespace launchpad::domain
