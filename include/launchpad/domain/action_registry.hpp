/**
 * @file action_registry.hpp
 * @brief First-class collection mapping hotkey sequences to domain Actions.
 * @details Implements Object Calisthenics rule 4 (First-class collections) to encapsulate
 *          the action routing table and provide safe lookup and mutation primitives.
 */

#pragma once

#include "launchpad/domain/action.hpp"
#include <map>
#include <optional>

namespace launchpad::domain {

/**
 * @brief First-class collection maintaining registered hotkey-action associations.
 */
class ActionRegistry {
public:
    /**
     * @brief Constructs an ActionRegistry with default built-in bindings (e.g. triple-shift screenshot).
     */
    ActionRegistry();

    /**
     * @brief Registers a key sequence to trigger the given action.
     * @param[in] sequence Key sequence chord.
     * @param[in] action Executable action payload and display label.
     */
    void register_action(core::KeySequence sequence, Action action);

    /**
     * @brief Looks up an action bound to the specified key sequence.
     * @param[in] sequence Key sequence chord to query.
     * @return Optional containing the matched Action, or std::nullopt if not registered.
     */
    [[nodiscard]] std::optional<Action> find_action(const core::KeySequence& sequence) const;

    /**
     * @brief Returns the total number of registered actions.
     * @return Count of registered hotkey mappings.
     */
    [[nodiscard]] size_t size() const noexcept;

    /**
     * @brief Clears all registered custom actions and restores default bindings.
     */
    void clear() noexcept;

private:
    core::KeySequence screenshot_sequence_;
    std::map<core::KeySequence, Action> actions_;
};

} // namespace launchpad::domain
