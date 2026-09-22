/**
 * @file display.hpp
 * @brief Abstract display and framebuffer interface for Kindle e-ink rendering.
 * @details Declares DisplayBounds, UpdateRegion, and the IDisplay interface, decoupling
 *          UI components from the Linux framebuffer (/dev/fb0) and Kindle e-ink ioctls.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include <cstdint>

namespace launchpad::hal {

/**
 * @brief Dimensions and color depth of an e-ink display screen.
 */
struct DisplayBounds {
    uint32_t width{0};         ///< Horizontal screen resolution in pixels (e.g. 600 or 824).
    uint32_t height{0};        ///< Vertical screen resolution in pixels (e.g. 800 or 1200).
    uint8_t bits_per_pixel{4}; ///< Color depth in bits per pixel (Kindle standard: 4bpp, 16 grayscales).
};

/**
 * @brief Rectangular bounding box for partial e-ink screen refresh updates.
 */
struct UpdateRegion {
    uint32_t x{0};      ///< Top-left X coordinate in pixels.
    uint32_t y{0};      ///< Top-left Y coordinate in pixels.
    uint32_t width{0};  ///< Width of the region in pixels.
    uint32_t height{0}; ///< Height of the region in pixels.
};

/**
 * @brief Abstract interface for pixel manipulation and e-ink display updates.
 */
class IDisplay {
public:
    virtual ~IDisplay() = default;

    /**
     * @brief Queries the geometry and color depth of the display.
     * @return DisplayBounds containing screen width, height, and bits per pixel.
     */
    [[nodiscard]] virtual DisplayBounds bounds() const noexcept = 0;

    /**
     * @brief Fills the entire display buffer with a uniform grayscale color.
     * @param[in] color Grayscale value (0x00 = black, 0xFF = white).
     */
    virtual void clear(uint8_t color = 0xFF) = 0;

    /**
     * @brief Sets the grayscale level of an individual pixel.
     * @param[in] x Horizontal pixel coordinate.
     * @param[in] y Vertical pixel coordinate.
     * @param[in] gray_level Grayscale value (0x00 = black, 0xFF = white).
     */
    virtual void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) = 0;

    /**
     * @brief Retrieves the grayscale level of an individual pixel.
     * @param[in] x Horizontal pixel coordinate.
     * @param[in] y Vertical pixel coordinate.
     * @return Grayscale level (0x00 to 0xFF).
     */
    [[nodiscard]] virtual uint8_t get_pixel(uint32_t x, uint32_t y) const = 0;

    /**
     * @brief Flushes pixel modifications in the specified region to the physical e-ink screen.
     * @param[in] region Rectangular boundary defining the updated area.
     * @return Result<void> indicating whether hardware refresh ioctl succeeded.
     */
    virtual core::Result<void> update(const UpdateRegion& region) = 0;
};

} // namespace launchpad::hal
