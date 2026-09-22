/**
 * @file units.hpp
 * @brief Strongly typed domain units and value objects.
 * @details Implements Object Calisthenics rule 3 (Wrap all primitives) and rule 4
 *          (First-class collections) for time intervals, raw key codes, and key sequences.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <compare>

namespace launchpad::core {

/**
 * @brief Strongly-typed millisecond duration value object.
 * @details Eliminates unit ambiguity in time-sensitive hotkey state transitions.
 */
struct Milliseconds {
    uint32_t value{0}; ///< Duration in integer milliseconds.

    /**
     * @brief Constructs a Milliseconds value object.
     * @param[in] ms Integer milliseconds duration (defaults to 0).
     */
    constexpr explicit Milliseconds(uint32_t ms = 0) noexcept : value(ms) {}

    /**
     * @brief Three-way spaceship comparison for ordering and equality.
     */
    constexpr auto operator<=>(const Milliseconds&) const noexcept = default;
};

/**
 * @brief Raw Linux evdev keycode identifier corresponding to KEY_* macros in <linux/input.h>.
 */
using RawKeyCode = int16_t;

/**
 * @brief First-class collection encapsulating an ordered sequence of raw key codes.
 * @details Represents a user-entered hotkey chord (e.g. Shift + Shift + Space).
 */
struct KeySequence {
    std::vector<RawKeyCode> codes; ///< Underlying vector of key codes.

    /**
     * @brief Default constructor creating an empty sequence.
     */
    KeySequence() = default;

    /**
     * @brief Constructs a KeySequence from an existing vector of codes.
     * @param[in] c Vector of raw key codes.
     */
    explicit KeySequence(std::vector<RawKeyCode> c) : codes(std::move(c)) {}

    /**
     * @brief Appends a keycode to the sequence.
     * @param[in] code Raw keycode to append.
     */
    void push(RawKeyCode code) {
        codes.push_back(code);
    }

    /**
     * @brief Clears all key codes from the sequence.
     */
    void clear() noexcept {
        codes.clear();
    }

    /**
     * @brief Checks if the key sequence contains no keys.
     * @return True if empty, false otherwise.
     */
    [[nodiscard]] bool empty() const noexcept {
        return codes.empty();
    }

    /**
     * @brief Returns the number of keys in the sequence.
     * @return Count of key codes.
     */
    [[nodiscard]] size_t size() const noexcept {
        return codes.size();
    }

    /**
     * @brief Three-way comparison operator for equality and sorting.
     */
    auto operator<=>(const KeySequence&) const noexcept = default;
};

} // namespace launchpad::core
