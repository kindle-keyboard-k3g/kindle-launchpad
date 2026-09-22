#pragma once

#include "launchpad/core/result.hpp"
#include <cstdint>

namespace launchpad::hal {

struct DisplayBounds {
    uint32_t width{0};
    uint32_t height{0};
    uint8_t bits_per_pixel{4};
};

struct UpdateRegion {
    uint32_t x{0};
    uint32_t y{0};
    uint32_t width{0};
    uint32_t height{0};
};

class IDisplay {
public:
    virtual ~IDisplay() = default;

    [[nodiscard]] virtual DisplayBounds bounds() const noexcept = 0;
    virtual void clear(uint8_t color = 0xFF) = 0;
    virtual void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) = 0;
    [[nodiscard]] virtual uint8_t get_pixel(uint32_t x, uint32_t y) const = 0;
    virtual core::Result<void> update(const UpdateRegion& region) = 0;
};

} // namespace launchpad::hal
