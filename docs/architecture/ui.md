# User Interface (UI) Subsystem Architecture

The **UI subsystem** (`launchpad::ui`) delivers visual feedback to the e-ink screen, implements font rasterization, and handles screen captures.

---

## 1. Embedded Font Rasterizer (`FontRenderer`)

Kindle Launchpad contains an embedded 12×22 monochrome bitmap font rasterizer in `FontRenderer`.

### 1.1 Glyph Encoding
- **Dimensions**: Each character glyph has a fixed bounding box of 12 pixels width by 22 pixels height.
- **Data Representation**: Glyphs are encoded as bit arrays, where 1 represents ink (foreground black) and 0 represents paper (background white).
- **Embedded Table**: Glyphs are mapped from standard 7-bit ASCII characters (values 32 through 126).

### 1.2 Rendering Operations
- `draw_char(display, x, y, char)`: Renders an individual character at the target $(X, Y)$ screen offset.
- `draw_string(display, x, y, text)`: Iterates through characters, advancing $X$ by 12 pixels per glyph.

---

## 2. Status Bar Feedback Overlay (`StatusBar`)

Because Kindle e-ink displays have no hardware compositing layers, drawing text directly onto `/dev/fb0` destroys the underlying book or menu pixels.

`StatusBar` implements a **non-destructive overlay** pattern:

```
[Idle Screen] ───────────┐
                         ▼
             [StatusBar::show_status("^[...]")]
                         │
                         ├─> 1. capture_background() (saves rect to memory)
                         ├─> 2. draw_bar_text() (renders glyphs into framebuffer)
                         └─> 3. display.update() (triggers fast partial refresh)
                         │
                         ▼
             [StatusBar::show_result(true)]
                         │
                         └─> Renders "Success!"
                         │
                         ▼ (Timer expires / dismiss)
             [StatusBar::hide_status()]
                         │
                         ├─> 1. restore_background() (writes saved pixels back)
                         └─> 2. display.update() (restores original screen)
```

### 2.1 Screen Placement
The status bar occupies the bottom-right corner of the screen:
- Height: Fixed 24 pixels (`STATUS_BAR_HEIGHT`).
- Width: Dynamically sized to fit the message text with 8 pixels of horizontal padding.
- Position: Anchored at $(X = \text{width} - \text{bar\_width}, Y = \text{height} - 24)$.

---

## 3. Framebuffer Screenshot Writer (`ScreenshotWriter`)

When triggered via triple-shift or custom hotkeys, `ScreenshotWriter::write_bmp` captures the display memory into an uncompressed BMP file:

### 3.1 BMP Format Specification
- **Format**: Standard Windows Bitmap (BMP) format.
- **Color Depth**: 8-bit indexed grayscale (256 color palette entries where $R=G=B$).
- **File Structure**:
  1. **`BITMAPFILEHEADER`** (14 bytes): Magic bytes `BM`, total file size, pixel array offset ($14 + 40 + 1024 = 1078$ bytes).
  2. **`BITMAPINFOHEADER`** (40 bytes): Image width, height, color planes ($1$), bit count ($8$), compression ($0$ uncompressed).
  3. **Color Palette** (1024 bytes): 256 quadruplets of $(B, G, R, 0)$ representing grays from black to white.
  4. **Pixel Data**: Rows stored in bottom-up order, with each scanline padded to a multiple of 4 bytes.

### 3.2 Output Destination
Files are written directly to USB user storage (`/mnt/us/screenshot_<timestamp>.bmp`), making them accessible over USB when connected to a computer.
