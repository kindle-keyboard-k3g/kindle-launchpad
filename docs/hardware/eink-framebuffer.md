# E-Ink Framebuffer & 4bpp Packed Pixel Layout

This document explains the technical details of the Amazon Kindle e-ink framebuffer (`/dev/fb0`), memory mapping, 4-bit-per-pixel (4bpp) packed grayscale format, and partial refresh updates.

---

## 1. Framebuffer Architecture & Memory Layout

Amazon Kindle devices use an electronic paper display (EPD) controller interfaced as a standard Linux framebuffer at `/dev/fb0`.

### 1.1 Screen Geometries & Memory Sizes

| Device Family | Pixel Resolution | Pixels Total | Buffer Size in Bytes ($N / 2$) |
| :--- | :--- | :--- | :--- |
| **Kindle Keyboard 3 (K3)** | 600 × 800 | 480,000 | 240,000 bytes (~234.4 KB) |
| **Kindle 2 / K2i** | 600 × 800 | 480,000 | 240,000 bytes (~234.4 KB) |
| **Kindle DX / DXG** | 824 × 1200 | 988,800 | 494,400 bytes (~482.8 KB) |

---

## 2. 4bpp Packed Grayscale Pixel Format

The e-ink controller represents each pixel using **4 bits** (16 discrete grayscale levels from `0x0` to `0xF`):
- `0x0` (0000b): Pure Black
- `0xF` (1111b): Pure White

### 2.1 Nibble Packing Scheme
Pixels are packed **two per byte** in row-major order:

```
 Byte N (address = (y * width + x) / 2):
 ┌───────────────┬───────────────┐
 │ Bits 7..4     │ Bits 3..0     │
 │ (High Nibble) │ (Low Nibble)  │
 ├───────────────┼───────────────┤
 │ Pixel (x, y)  │ Pixel (x+1, y)│  where x is even
 └───────────────┴───────────────┘
```

### 2.2 Pixel Access Algorithms

#### Reading a Pixel (`get_pixel`)
```cpp
uint8_t KindleDisplay::get_pixel(uint32_t x, uint32_t y) const {
    size_t index = (y * bounds_.width + x) / 2;
    uint8_t byte_val = handle_.memory[index];

    if ((x & 1) == 0) {
        // Even x coordinate: high nibble
        return (byte_val >> 4) & 0x0F;
    } else {
        // Odd x coordinate: low nibble
        return byte_val & 0x0F;
    }
}
```

#### Writing a Pixel (`set_pixel`)
```cpp
void KindleDisplay::set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) {
    size_t index = (y * bounds_.width + x) / 2;
    uint8_t quantized = gray_level >> 4; // Scale 8-bit value (0..255) to 4-bit (0..15)

    if ((x & 1) == 0) {
        // Even x: update high nibble, preserve low nibble
        handle_.memory[index] = (handle_.memory[index] & 0x0F) | (quantized << 4);
    } else {
        // Odd x: update low nibble, preserve high nibble
        handle_.memory[index] = (handle_.memory[index] & 0xF0) | (quantized & 0x0F);
    }
}
```

---

## 3. Memory-Mapping (`mmap`) & Synchronization

Kindle Launchpad accesses the framebuffer via direct memory mapping:

```cpp
void KindleDisplay::open_framebuffer(std::string_view fb_device_path) {
    handle_.file_descriptor = ::open(fb_device_path.data(), O_RDWR);
    handle_.size = (bounds_.width * bounds_.height) / 2;

    void* ptr = ::mmap(
        nullptr,
        handle_.size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        handle_.file_descriptor,
        0
    );

    handle_.memory = static_cast<uint8_t*>(ptr);
    handle_.is_mmapped = true;
}
```

---

## 4. UI Overlay Save & Restore Pattern

Because Kindle e-ink screens do not have hardware graphics compositing layers, the `StatusBar` widget implements software background save and restore:

```
[Screen Content Active]
         │
         ▼
[StatusBar::save_background()] ──> Copies 4bpp pixel rect to RAM buffer
         │
         ▼
[StatusBar::draw_text()]       ──> Blits font glyphs into /dev/fb0
         │
         ▼
[KindleDisplay::update()]      ──> Partial refresh ioctl triggers screen flash
         │
         ▼ (Action completes or timer expires)
[StatusBar::restore_background()]──> Copies saved pixels back into /dev/fb0
         │
         ▼
[KindleDisplay::update()]      ──> Restores original book text cleanly
```
This guarantees that temporary status messages leave zero visual residue or screen corruption after dismissal.
