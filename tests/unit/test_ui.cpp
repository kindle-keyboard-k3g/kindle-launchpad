#include "launchpad/ui/font.hpp"
#include "launchpad/ui/status_bar.hpp"
#include "launchpad/ui/screenshot_writer.hpp"
#include "launchpad/hal/mock_display.hpp"
#include "test_framework.hpp"
#include <fstream>
#include <cstdio>

using namespace launchpad::core;
using namespace launchpad::hal;
using namespace launchpad::ui;

TEST_CASE("UI - FontRenderer Metrics") {
    FontRenderer font;
    CHECK(font.glyph_width() == 12);
    CHECK(font.glyph_height() == 22);
}

TEST_CASE("UI - FontRenderer Draw Char") {
    DisplayBounds bounds{100, 50, 4};
    MockDisplay display(bounds);
    FontRenderer font;

    // Render 'A' at (10, 10)
    font.draw_char(display, 10, 10, 'A');

    // Verify that some pixels in the glyph box were drawn (not all white)
    bool has_drawn_pixels = false;
    for (uint32_t y = 10; y < 10 + 22; ++y) {
        for (uint32_t x = 10; x < 10 + 12; ++x) {
            if (display.get_pixel(x, y) != 0xFF) {
                has_drawn_pixels = true;
                break;
            }
        }
    }
    CHECK(has_drawn_pixels);
}

TEST_CASE("UI - FontRenderer Draw String") {
    DisplayBounds bounds{200, 50, 4};
    MockDisplay display(bounds);
    FontRenderer font;

    font.draw_string(display, 10, 10, "Hi");

    bool has_h = false;
    for (uint32_t x = 10; x < 22; ++x) {
        if (display.get_pixel(x, 15) != 0xFF) has_h = true;
    }
    CHECK(has_h);
}

TEST_CASE("UI - StatusBar Show Status") {
    DisplayBounds bounds{600, 800, 4};
    MockDisplay display(bounds);
    FontRenderer font;
    StatusBar status_bar(display, font);

    status_bar.show_status("^[Shift Space]");

    REQUIRE_FALSE(display.update_regions().empty());
    const auto& last_update = display.update_regions().back();
    CHECK(last_update.y >= 800 - 24);
    CHECK(last_update.height == 24);
}

TEST_CASE("UI - StatusBar Save and Restore Background") {
    DisplayBounds bounds{600, 800, 4};
    MockDisplay display(bounds);
    FontRenderer font;
    StatusBar status_bar(display, font);

    display.clear(0xAA);
    status_bar.show_status("^[Shift Space]");
    status_bar.show_result(true);
    status_bar.hide_status();

    CHECK(display.get_pixel(50, 800 - 10) == 0xAA);
}

TEST_CASE("UI - ScreenshotWriter") {
    DisplayBounds bounds{16, 16, 4};
    MockDisplay display(bounds);
    display.clear(0x00);

    std::string test_file = "/tmp/test_launchpad_screenshot.bmp";
    auto res = ScreenshotWriter::write_bmp(display, test_file);
    CHECK(res.is_ok());

    std::ifstream file(test_file, std::ios::binary);
    REQUIRE(file.is_open());

    char magic[2];
    file.read(magic, 2);
    CHECK(magic[0] == 'B');
    CHECK(magic[1] == 'M');

    file.close();
    std::remove(test_file.c_str());
}
