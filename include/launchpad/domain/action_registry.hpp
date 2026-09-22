#pragma once

#include "launchpad/domain/action.hpp"
#include <map>
#include <optional>

namespace launchpad::domain {

class ActionRegistry {
public:
    ActionRegistry();

    void register_action(core::KeySequence sequence, Action action);
    [[nodiscard]] std::optional<Action> find_action(const core::KeySequence& sequence) const;
    [[nodiscard]] size_t size() const noexcept;
    void clear() noexcept;

private:
    core::KeySequence screenshot_sequence_;
    std::map<core::KeySequence, Action> actions_;
};

} // namespace launchpad::domain
