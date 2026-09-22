#pragma once

#include "launchpad/core/units.hpp"
#include <cstdint>
#include <optional>

namespace launchpad::domain {

enum class SendMode : uint8_t {
    Normal = 0,
    Shift = 1,
    Alt = 2
};

enum class KindleModel : uint8_t {
    KindleDX = 0,
    Kindle3 = 1
};

struct SymbolNavigation {
    int8_t horizontal_steps{0};
    int8_t vertical_steps{0};
};

struct AsciiKeyMapping {
    core::RawKeyCode code{0};
    SendMode send_mode{SendMode::Normal};
    std::optional<SymbolNavigation> symbol_navigation{std::nullopt};
};

class AsciiTranslator {
public:
    explicit AsciiTranslator(KindleModel model = KindleModel::KindleDX) noexcept;

    [[nodiscard]] AsciiKeyMapping translate(char c) const noexcept;
    [[nodiscard]] KindleModel model() const noexcept;
    void set_model(KindleModel model) noexcept;

private:
    KindleModel model_;
};

} // namespace launchpad::domain
