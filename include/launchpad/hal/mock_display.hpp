#pragma once

#include "launchpad/hal/display.hpp"
#include <vector>

namespace launchpad::hal {

struct MockDisplayState {
    std::vector<uint8_t> pixels;
    std::vector<UpdateRegion> updates;
};

class MockDisplay : public IDisplay {
public:
    explicit MockDisplay(DisplayBounds bounds)
        : bounds_(bounds) {
        state_.pixels.resize(static_cast<size_t>(bounds_.width) * bounds_.height, 0xFF);
    }

    [[nodiscard]] DisplayBounds bounds() const noexcept override {
        return bounds_;
    }

    void clear(uint8_t color = 0xFF) override {
        std::fill(state_.pixels.begin(), state_.pixels.end(), color);
    }

    void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) override {
        if (x >= bounds_.width || y >= bounds_.height) {
            return;
        }
        state_.pixels[y * bounds_.width + x] = gray_level;
    }

    [[nodiscard]] uint8_t get_pixel(uint32_t x, uint32_t y) const override {
        if (x >= bounds_.width || y >= bounds_.height) {
            return 0xFF;
        }
        return state_.pixels[y * bounds_.width + x];
    }

    core::Result<void> update(const UpdateRegion& region) override {
        state_.updates.push_back(region);
        return core::Result<void>();
    }

    [[nodiscard]] const std::vector<UpdateRegion>& update_regions() const noexcept {
        return state_.updates;
    }

private:
    DisplayBounds bounds_;
    MockDisplayState state_;
};

} // namespace launchpad::hal
