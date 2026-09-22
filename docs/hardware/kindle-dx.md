# Kindle DX & DX Graphite Hardware Profile

This document details the hardware specifications, kernel interfaces, device nodes, and input subsystem behavior for the **Amazon Kindle DX** family.

---

## 1. Device Variants & Identifiers

| Model | Market Name | Serial Prefix | Screen Technology | Wireless |
| :--- | :--- | :--- | :--- | :--- |
| **DX** | Kindle DX (US) | `B004` | 9.7" E-Ink Vizplex | Cellular (Sprint CDMA / EV-DO) |
| **DXi** | Kindle DX International | `B005` | 9.7" E-Ink Vizplex | Cellular (AT&T 3G) |
| **DXG** | Kindle DX Graphite | `B009` | 9.7" E-Ink Pearl | Cellular (AT&T 3G) |

---

## 2. Hardware Specifications

- **SoC**:
  - DX / DXi: Freescale i.MX31 Multimedia Applications Processor (ARM1136JF-S @ 532 MHz)
  - DXG: Freescale i.MX353 Applications Processor (ARM1136JF-S @ 532 MHz)
- **Memory**: 128 MB Mobile DDR SDRAM
- **Storage**: 4 GB internal flash memory (`/mnt/us` FAT32 mount)
- **Display**: 9.7-inch large-format e-ink panel
  - Resolution: 824 × 1200 pixels
  - Density: 150 DPI
  - Color Depth: 4bpp (16 levels of gray), packed as 2 pixels per byte
  - Framebuffer Memory Size: $824 \times 1200 \times 0.5 = 494,400$ bytes
- **Orientation Sensor**: Built-in 3-axis accelerometer for automatic screen rotation (present on `/dev/input/event2`).

---

## 3. Physical Keyboard Matrix & Scancodes

Unlike the smaller 6-inch Kindle models, the Kindle DX features a full physical QWERTY keyboard including a **dedicated numeric row** (`1` through `0` keys):

```
[ 1 ][ 2 ][ 3 ][ 4 ][ 5 ][ 6 ][ 7 ][ 8 ][ 9 ][ 0 ]
 [ Q ][ W ][ E ][ R ][ T ][ Y ][ U ][ I ][ O ][ P ]
  [ A ][ S ][ D ][ F ][ G ][ H ][ J ][ K ][ L ][Del]
   [ Z ][ X ][ C ][ V ][ B ][ N ][ M ][ . ][ / ][Enter]
[Shift][Alt][      Spacebar      ][Sym][Aa][Home]
```

All scancodes on the Kindle DX directly correspond to the canonical `launchpad::domain::key_codes::KPKEY_*` constants:
- Numbers `1`–`0`: Scancodes 2–11
- QWERTY `Q`–`P`: Scancodes 16–25
- QWERTY `A`–`L`: Scancodes 30–38
- QWERTY `Z`–`M`: Scancodes 44–50
- `Shift`: 42, `Alt`: 56, `Space`: 57, `Enter`: 28
- 5-Way Joystick: `KPKEY_FW_LEFT` (105), `KPKEY_FW_RIGHT` (106), `KPKEY_FW_UP` (122), `KPKEY_FW_DOWN` (123), `KPKEY_FW_PRESS` (92)

---

## 4. Input Subsystem Nodes

| Device Node | Kernel Device Name | Description |
| :--- | :--- | :--- |
| `/dev/input/event0` | `mx31_kpp` / `matrix-keypad` | Physical 46-key keyboard matrix |
| `/dev/input/event1` | `fiveway` | 5-way navigation joystick |
| `/dev/input/event2` | `accelerometer` | 3-axis orientation sensor |

---

## 5. Screen Geometry & Status Bar Positioning

Because the Kindle DX has an $824 \times 1200$ screen (compared to the $600 \times 800$ screen of the K3), `launchpad::ui::StatusBar` dynamically reads display boundaries from `hal::IDisplay::bounds()`:

```cpp
core::DisplayBounds bounds = display.bounds();
// bounds.width  = 824
// bounds.height = 1200
```
This ensures the visual feedback status bar is positioned in the bottom-right corner regardless of whether Launchpad is running on a Kindle DX or Kindle Keyboard 3.
