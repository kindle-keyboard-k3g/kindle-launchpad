/**
 * @file kindle_display.hpp
 * @brief Concrete Linux framebuffer driver for Kindle e-ink displays.
 * @details Maps /dev/fb0 into user memory via mmap(), manages 4bpp packed pixel layouts,
 *          and triggers hardware screen refreshes via FBIO_EINK_UPDATE_DISPLAY_AREA ioctls.
 */

#pragma once

#include "launchpad/hal/display.hpp"
#include <string_view>

namespace launchpad::hal {

/**
 * @brief Low-level state of the memory-mapped Linux framebuffer.
 */
struct FramebufferHandle {
    int file_descriptor{-1};   ///< Open file descriptor for /dev/fb0.
    uint8_t* memory{nullptr};  ///< Pointer to mmap'd framebuffer memory.
    size_t size{0};            ///< Total size of mapped memory buffer in bytes.
    bool is_mmapped{false};    ///< Whether memory mapping was successfully established.
};

/**
 * @brief Concrete IDisplay driver interfacing with the Linux e-ink framebuffer (/dev/fb0).
 */
class KindleDisplay : public IDisplay {
public:
    /**
     * @brief Constructs a KindleDisplay driver opening the specified framebuffer node.
     * @param[in] fb_device_path Device path (defaults to "/dev/fb0").
     */
    explicit KindleDisplay(std::string_view fb_device_path = "/dev/fb0");

    /**
     * @brief Destructor unmapping framebuffer memory and closing the device file.
     */
    ~KindleDisplay() override;

    KindleDisplay(const KindleDisplay&) = delete;
    KindleDisplay& operator=(const KindleDisplay&) = delete;

    /**
     * @brief Returns the geometry and color depth of the active display.
     * @return DisplayBounds containing width, height, and bits per pixel.
     */
    [[nodiscard]] DisplayBounds bounds() const noexcept override;

    /**
     * @brief Fills the entire framebuffer with a uniform grayscale byte.
     * @param[in] color Grayscale value (0xFF = white, 0x00 = black).
     */
    void clear(uint8_t color = 0xFF) override;

    /**
     * @brief Writes a 4bpp pixel to the memory-mapped framebuffer.
     * @param[in] x Horizontal pixel coordinate.
     * @param[in] y Vertical pixel coordinate.
     * @param[in] gray_level 8-bit grayscale value (quantized to 4 bits).
     */
    void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) override;

    /**
     * @brief Reads a 4bpp pixel from the memory-mapped framebuffer.
     * @param[in] x Horizontal pixel coordinate.
     * @param[in] y Vertical pixel coordinate.
     * @return 8-bit expanded grayscale value.
     */
    [[nodiscard]] uint8_t get_pixel(uint32_t x, uint32_t y) const override;

    /**
     * @brief Triggers an e-ink partial refresh ioctl for the specified bounding region.
     * @param[in] region Rectangular boundary defining the updated area.
     * @return Result<void> indicating success or ioctl failure.
     */
    core::Result<void> update(const UpdateRegion& region) override;

private:
    DisplayBounds bounds_{600, 800, 4};
    FramebufferHandle handle_{};

    void open_framebuffer(std::string_view fb_device_path);
    void close_framebuffer();
};

} // namespace launchpad::hal
