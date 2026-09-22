/**
 * @file mock_display.hpp
 * @brief In-memory mock display driver for host testing.
 * @details Implements IDisplay with an in-memory byte buffer representing pixels,
 *          capturing pixel drawing and screen refresh update regions for automated test assertions.
 */

#pragma once

#include "launchpad/hal/display.hpp"
#include <vector>

namespace launchpad::hal {

/**
 * @brief Internal in-memory pixel buffer and update history of the MockDisplay.
 */
struct MockDisplayState {
    std::vector<uint8_t> pixels;      ///< 8-bit grayscale pixel array (width * height).
    std::vector<UpdateRegion> updates;///< History of regions flushed via update().
};

/**
 * @brief In-memory IDisplay test double for verifying UI rendering without hardware.
 */
class MockDisplay : public IDisplay {
public:
    /**
     * @brief Constructs a MockDisplay with specified screen dimensions.
     * @param[in] bounds DisplayBounds specifying width, height, and color depth.
     */
    explicit MockDisplay(DisplayBounds bounds)
        : bounds_(bounds) {
        state_.pixels.resize(static_cast<size_t>(bounds_.width) * bounds_.height, 0xFF);
    }

    /**
     * @brief Queries the configured mock screen bounds.
     * @return DisplayBounds struct.
     */
    [[nodiscard]] DisplayBounds bounds() const noexcept override {
        return bounds_;
    }

    /**
     * @brief Fills the entire in-memory pixel buffer with the given color.
     * @param[in] color Grayscale value (defaults to 0xFF white).
     */
    void clear(uint8_t color = 0xFF) override {
        std::fill(state_.pixels.begin(), state_.pixels.end(), color);
    }

    /**
     * @brief Sets the grayscale level of an individual pixel in memory.
     * @param[in] x Horizontal pixel coordinate.
     * @param[in] y Vertical pixel coordinate.
     * @param[in] gray_level Grayscale value (0x00 to 0xFF).
     */
    void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) override {
        if (x >= bounds_.width || y >= bounds_.height) {
            return;
        }
        state_.pixels[y * bounds_.width + x] = gray_level;
    }

    /**
     * @brief Reads the grayscale level of a pixel from memory.
     * @param[in] x Horizontal coordinate.
     * @param[in] y Vertical coordinate.
     * @return Grayscale value or 0xFF if out of bounds.
     */
    [[nodiscard]] uint8_t get_pixel(uint32_t x, uint32_t y) const override {
        if (x >= bounds_.width || y >= bounds_.height) {
            return 0xFF;
        }
        return state_.pixels[y * bounds_.width + x];
    }

    /**
     * @brief Records the updated bounding box into the update history.
     * @param[in] region Rectangular region that was refreshed.
     * @return Successful Result<void>.
     */
    core::Result<void> update(const UpdateRegion& region) override {
        state_.updates.push_back(region);
        return core::Result<void>();
    }

    /**
     * @brief Returns the log of all update regions flushed during testing.
     * @return Const reference to vector of UpdateRegion entries.
     */
    [[nodiscard]] const std::vector<UpdateRegion>& update_regions() const noexcept {
        return state_.updates;
    }

private:
    DisplayBounds bounds_;
    MockDisplayState state_;
};

} // namespace launchpad::hal
