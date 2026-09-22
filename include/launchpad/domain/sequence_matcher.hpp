/**
 * @file sequence_matcher.hpp
 * @brief Finite state machine tracking hotkey chord matching and transitions.
 * @details Implements state transitions across Idle, Introducer armed, and Active Key Capturing
 *          states, handling inter-key timeouts (hot_interval) and sequence terminations.
 */

#pragma once

#include "launchpad/core/units.hpp"
#include "launchpad/domain/key_definition.hpp"

namespace launchpad::domain {

/**
 * @brief Key event transition direction reported by the Linux input subsystem.
 */
enum class KeyEventType : uint8_t {
    Pressed = 1,  ///< Key depressed (EV_KEY value 1).
    Released = 0  ///< Key released (EV_KEY value 0).
};

/**
 * @brief Operational state of the SequenceMatcher finite state machine.
 */
enum class MatcherState : uint8_t {
    Idle,      ///< Awaiting the Introducer key (e.g. Shift); input passes through to OS.
    Capturing  ///< Actively accumulating hotkey chord keys; input is grabbed exclusively.
};

/**
 * @brief Action directive resulting from processing a key event.
 */
enum class MatcherActionType : uint8_t {
    None,             ///< No state change or action required.
    StartCapture,     ///< Introducer sequence triggered; daemon must grab hardware input.
    KeyAppended,      ///< Key added to active sequence buffer; reset timeout timer.
    SequenceComplete, ///< Trailer key pressed or terminal sequence recognized; execute action.
    Cancelled         ///< Timeout expired or non-matching key pressed; release input grab.
};

/**
 * @brief Evaluation result produced by SequenceMatcher state transitions.
 */
struct MatcherResult {
    MatcherActionType type{MatcherActionType::None}; ///< High-level directive for the application loop.
    core::KeySequence sequence{};                   ///< Accumulated key sequence at the time of the event.
};

/**
 * @brief Immutable configuration defining sequence boundaries.
 */
struct MatcherConfig {
    core::RawKeyCode introducer{key_codes::KPKEY_SHIFT}; ///< Key triggering transition to Capturing.
    core::RawKeyCode trailer{key_codes::KPKEY_ENTER};    ///< Key triggering completion of Capturing.
    const KeyTable* key_table{nullptr};                  ///< KeyTable for model substitution.
};

/**
 * @brief Internal mutable state tracking active key accumulation.
 */
struct MatcherContext {
    MatcherState state{MatcherState::Idle}; ///< Current FSM state.
    int keys_down{0};                       ///< Number of currently depressed keys.
    bool introducer_armed{false};           ///< Whether the introducer key was pressed and released.
    core::KeySequence sequence{};           ///< Currently accumulated key codes.
};

/**
 * @brief Pure finite state machine managing hotkey chord accumulation and timeouts.
 */
class SequenceMatcher {
public:
    /**
     * @brief Constructs a SequenceMatcher with the given boundary configuration.
     * @param[in] config MatcherConfig defining introducer and trailer keycodes.
     * @param[in] table Optional KeyTable pointer for key substitution.
     */
    explicit SequenceMatcher(MatcherConfig config, const KeyTable* table = nullptr) noexcept;

    /**
     * @brief Processes an incoming hardware key event and transitions state.
     * @param[in] code Raw evdev key code.
     * @param[in] type Whether key was pressed or released.
     * @return MatcherResult indicating what action the application should perform.
     */
    [[nodiscard]] MatcherResult process_key_event(core::RawKeyCode code, KeyEventType type) noexcept;

    /**
     * @brief Notifies the matcher that the inter-key timeout (hot_interval) has elapsed.
     * @return MatcherResult indicating Cancelled and resetting state to Idle.
     */
    [[nodiscard]] MatcherResult on_timeout() noexcept;

    /**
     * @brief Manually resets the state machine to Idle.
     * @return MatcherResult with type Cancelled.
     */
    [[nodiscard]] MatcherResult cancel() noexcept;

    /**
     * @brief Queries the current state of the matcher.
     * @return Current MatcherState (Idle or Capturing).
     */
    [[nodiscard]] MatcherState state() const noexcept;

    /**
     * @brief Returns the key sequence accumulated so far.
     * @return Const reference to active KeySequence.
     */
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
