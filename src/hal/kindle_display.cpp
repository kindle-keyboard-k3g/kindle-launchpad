#include "launchpad/hal/kindle_display.hpp"
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

typedef unsigned char u8;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "linux/einkfb.h"
#pragma GCC diagnostic pop

namespace launchpad::hal {

KindleDisplay::KindleDisplay(std::string_view fb_device_path) {
    open_framebuffer(fb_device_path);
}

KindleDisplay::~KindleDisplay() {
    close_framebuffer();
}

void KindleDisplay::open_framebuffer(std::string_view fb_device_path) {
    int fd = open(std::string(fb_device_path).c_str(), O_RDWR);
    if (fd >= 0) {
        fb_var_screeninfo vinfo{};
        if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == 0 && vinfo.xres > 0 && vinfo.yres > 0) {
            bounds_ = DisplayBounds{vinfo.xres, vinfo.yres, static_cast<uint8_t>(vinfo.bits_per_pixel)};
        }
        handle_.size = (bounds_.width * bounds_.height * bounds_.bits_per_pixel) / 8;
        void* mapped = mmap(nullptr, handle_.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mapped != MAP_FAILED) {
            handle_.file_descriptor = fd;
            handle_.memory = static_cast<uint8_t*>(mapped);
            handle_.is_mmapped = true;
            return;
        }
        close(fd);
    }
    handle_.size = (bounds_.width * bounds_.height * bounds_.bits_per_pixel) / 8;
    handle_.memory = new uint8_t[handle_.size];
    std::fill_n(handle_.memory, handle_.size, 0xFF);
}

void KindleDisplay::close_framebuffer() {
    if (handle_.is_mmapped) {
        munmap(handle_.memory, handle_.size);
        close(handle_.file_descriptor);
        return;
    }
    delete[] handle_.memory;
    handle_.memory = nullptr;
}

DisplayBounds KindleDisplay::bounds() const noexcept {
    return bounds_;
}

void KindleDisplay::clear(uint8_t color) {
    uint8_t nibble = static_cast<uint8_t>(color >> 4);
    uint8_t packed = static_cast<uint8_t>((nibble << 4) | nibble);
    std::fill_n(handle_.memory, handle_.size, packed);
}

void KindleDisplay::set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) {
    if (x >= bounds_.width || y >= bounds_.height || !handle_.memory) return;
    uint32_t stride = (bounds_.width + 1) / 2;
    size_t byte_index = y * stride + (x / 2);
    uint8_t nibble = static_cast<uint8_t>((255 - gray_level) / 17);

    if (x % 2 == 0) {
        handle_.memory[byte_index] = static_cast<uint8_t>((handle_.memory[byte_index] & 0x0F) | (nibble << 4));
        return;
    }
    handle_.memory[byte_index] = static_cast<uint8_t>((handle_.memory[byte_index] & 0xF0) | (nibble & 0x0F));
}

uint8_t KindleDisplay::get_pixel(uint32_t x, uint32_t y) const {
    if (x >= bounds_.width || y >= bounds_.height || !handle_.memory) return 0xFF;
    uint32_t stride = (bounds_.width + 1) / 2;
    size_t byte_index = y * stride + (x / 2);
    uint8_t raw = handle_.memory[byte_index];
    uint8_t nibble = (x % 2 == 0) ? (raw >> 4) : (raw & 0x0F);
    return static_cast<uint8_t>(255 - (nibble * 17));
}

core::Result<void> KindleDisplay::update(const UpdateRegion& region) {
    if (handle_.file_descriptor < 0) return core::Result<void>();
    update_area_t ua{};
    ua.x1 = static_cast<int>(region.x);
    ua.y1 = static_cast<int>(region.y);
    ua.x2 = static_cast<int>(region.x + region.width);
    ua.y2 = static_cast<int>(region.y + region.height);
    ua.which_fx = fx_update_partial;
    ua.buffer = nullptr;
    ioctl(handle_.file_descriptor, FBIO_EINK_UPDATE_DISPLAY_AREA, &ua);
    return core::Result<void>();
}

} // namespace launchpad::hal
