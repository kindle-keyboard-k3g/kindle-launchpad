#pragma once

#include "launchpad/hal/display.hpp"
#include <string_view>

namespace launchpad::hal {

struct FramebufferHandle {
    int file_descriptor{-1};
    uint8_t* memory{nullptr};
    size_t size{0};
    bool is_mmapped{false};
};

class KindleDisplay : public IDisplay {
public:
    explicit KindleDisplay(std::string_view fb_device_path = "/dev/fb0");
    ~KindleDisplay() override;

    KindleDisplay(const KindleDisplay&) = delete;
    KindleDisplay& operator=(const KindleDisplay&) = delete;

    [[nodiscard]] DisplayBounds bounds() const noexcept override;
    void clear(uint8_t color = 0xFF) override;
    void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) override;
    [[nodiscard]] uint8_t get_pixel(uint32_t x, uint32_t y) const override;
    core::Result<void> update(const UpdateRegion& region) override;

private:
    DisplayBounds bounds_{600, 800, 4};
    FramebufferHandle handle_{};

    void open_framebuffer(std::string_view fb_device_path);
    void close_framebuffer();
};

} // namespace launchpad::hal
