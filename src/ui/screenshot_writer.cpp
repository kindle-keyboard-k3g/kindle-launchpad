#include "launchpad/ui/screenshot_writer.hpp"
#include <fstream>
#include <vector>

namespace launchpad::ui {

void ScreenshotWriter::write_headers(std::ofstream& file, uint32_t width, uint32_t height, uint32_t stride) {
    uint32_t offset = 14 + 40 + (16 * 4);
    uint32_t file_size = offset + (stride * height);

    // 14-byte BMP Header
    file.write("BM", 2);
    file.write(reinterpret_cast<const char*>(&file_size), 4);
    uint32_t reserved = 0;
    file.write(reinterpret_cast<const char*>(&reserved), 4);
    file.write(reinterpret_cast<const char*>(&offset), 4);

    // 40-byte DIB Header
    uint32_t dib_size = 40;
    int32_t w = static_cast<int32_t>(width);
    int32_t h = static_cast<int32_t>(height);
    uint16_t planes = 1;
    uint16_t bpp = 4;
    uint32_t compression = 0;
    uint32_t img_size = stride * height;
    int32_t dpi = 2835;
    uint32_t colors = 16;

    file.write(reinterpret_cast<const char*>(&dib_size), 4);
    file.write(reinterpret_cast<const char*>(&w), 4);
    file.write(reinterpret_cast<const char*>(&h), 4);
    file.write(reinterpret_cast<const char*>(&planes), 2);
    file.write(reinterpret_cast<const char*>(&bpp), 2);
    file.write(reinterpret_cast<const char*>(&compression), 4);
    file.write(reinterpret_cast<const char*>(&img_size), 4);
    file.write(reinterpret_cast<const char*>(&dpi), 4);
    file.write(reinterpret_cast<const char*>(&dpi), 4);
    file.write(reinterpret_cast<const char*>(&colors), 4);
    file.write(reinterpret_cast<const char*>(&colors), 4);
}

void ScreenshotWriter::write_palette(std::ofstream& file) {
    for (int i = 0; i < 16; ++i) {
        uint8_t shade = static_cast<uint8_t>(255 - (i * 17));
        uint8_t entry[4] = {shade, shade, shade, 0};
        file.write(reinterpret_cast<const char*>(entry), 4);
    }
}

void ScreenshotWriter::write_pixel_rows(std::ofstream& file, const hal::IDisplay& display, uint32_t width, uint32_t height, uint32_t stride) {
    std::vector<uint8_t> row_buffer(stride, 0);

    for (int32_t y = static_cast<int32_t>(height) - 1; y >= 0; --y) {
        std::fill(row_buffer.begin(), row_buffer.end(), 0);
        for (uint32_t x = 0; x < width; ++x) {
            uint8_t raw = display.get_pixel(x, static_cast<uint32_t>(y));
            uint8_t palette_idx = static_cast<uint8_t>((255 - raw) / 17);
            if (x % 2 == 0) {
                row_buffer[x / 2] |= static_cast<uint8_t>(palette_idx << 4);
            }
            if (x % 2 != 0) {
                row_buffer[x / 2] |= (palette_idx & 0x0F);
            }
        }
        file.write(reinterpret_cast<const char*>(row_buffer.data()), static_cast<std::streamsize>(stride));
    }
}

core::Result<void> ScreenshotWriter::write_bmp(const hal::IDisplay& display, std::string_view file_path) {
    std::ofstream file(std::string(file_path), std::ios::binary);
    if (!file.is_open()) {
        return core::Result<void>(core::Error("Could not open destination file for BMP"));
    }

    auto bounds = display.bounds();
    uint32_t stride = ((bounds.width * 4 + 31) / 32) * 4;

    write_headers(file, bounds.width, bounds.height, stride);
    write_palette(file);
    write_pixel_rows(file, display, bounds.width, bounds.height, stride);
    return core::Result<void>();
}

} // namespace launchpad::ui
