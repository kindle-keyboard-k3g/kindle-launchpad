#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <compare>

namespace launchpad::core {

struct Milliseconds {
    uint32_t value{0};

    constexpr explicit Milliseconds(uint32_t ms = 0) noexcept : value(ms) {}

    constexpr auto operator<=>(const Milliseconds&) const noexcept = default;
};

using RawKeyCode = int16_t;

struct KeySequence {
    std::vector<RawKeyCode> codes;

    KeySequence() = default;
    explicit KeySequence(std::vector<RawKeyCode> c) : codes(std::move(c)) {}

    void push(RawKeyCode code) {
        codes.push_back(code);
    }

    void clear() noexcept {
        codes.clear();
    }

    [[nodiscard]] bool empty() const noexcept {
        return codes.empty();
    }

    [[nodiscard]] size_t size() const noexcept {
        return codes.size();
    }

    auto operator<=>(const KeySequence&) const noexcept = default;
};

} // namespace launchpad::core
