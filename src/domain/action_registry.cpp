#include "launchpad/domain/action_registry.hpp"

namespace launchpad::domain {

ActionRegistry::ActionRegistry()
    : screenshot_sequence_({key_codes::KPKEY_SHIFT, key_codes::KPKEY_SHIFT}) {}

void ActionRegistry::register_action(core::KeySequence sequence, Action action) {
    if (actions_.contains(sequence)) {
        return;
    }
    actions_.emplace(std::move(sequence), std::move(action));
}

std::optional<Action> ActionRegistry::find_action(const core::KeySequence& sequence) const {
    if (sequence == screenshot_sequence_) {
        return Action{"[Screenshot]", ScreenshotAction{}};
    }
    auto it = actions_.find(sequence);
    if (it == actions_.end()) {
        return std::nullopt;
    }
    return it->second;
}

size_t ActionRegistry::size() const noexcept {
    return actions_.size();
}

void ActionRegistry::clear() noexcept {
    actions_.clear();
}

} // namespace launchpad::domain
