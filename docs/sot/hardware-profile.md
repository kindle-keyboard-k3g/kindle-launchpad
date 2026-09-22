# Source of Truth: Kindle Hardware Profiles

This document serves as the authoritative **Source of Truth** for physical hardware specifications, memory layouts, Linux device nodes, and display characteristics across all supported Kindle e-ink devices.

---

## 1. Device Hardware Matrix

| Parameter | Kindle 2 (K2 / K2i) | Kindle DX (DX / DXI / DXG) | Kindle Keyboard (K3G / K3W / K3GB) |
| :--- | :--- | :--- | :--- |
| **Model Codes** | `k2`, `k2i` | `dx`, `dxi`, `dxg` | `k3g`, `k3w`, `k3gb` |
| **Release Year** | 2009 | 2009 (DX/DXI), 2010 (DXG) | 2010 |
| **SoC / Processor** | Freescale i.MX31 (ARMv6) | Freescale i.MX31 (ARMv6) | Freescale i.MX353 (ARMv6) |
| **CPU Clock** | 399 MHz | 399 MHz | 532 MHz |
| **Instruction Set** | ARMv6 (EABI5, soft-float) | ARMv6 (EABI5, soft-float) | ARMv6 (EABI5, soft-float) |
| **RAM** | 64 MB (K2) / 128 MB (K2i) | 128 MB | 256 MB |
| **Internal Storage** | 2 GB eMMC | 3.3 GB eMMC | 4 GB eMMC |
| **Kernel Version** | Linux 2.6.26 | Linux 2.6.26 | Linux 2.6.26 |
| **Native Glibc** | Glibc 2.5 | Glibc 2.5 | Glibc 2.9 |

---

## 2. Display & Framebuffer Specifications

| Specification | Kindle 2 (K2 / K2i) | Kindle DX (DX / DXI / DXG) | Kindle Keyboard (K3) |
| :--- | :--- | :--- | :--- |
| **Display Panel** | 6.0" E-Ink Vizplex | 9.7" E-Ink Vizplex / Pearl (DXG) | 6.0" E-Ink Pearl |
| **Resolution** | 600 × 800 pixels | 824 × 1200 pixels | 600 × 800 pixels |
| **Pixel Density** | 167 PPI | 150 PPI | 167 PPI |
| **Pixel Format** | 4bpp packed grayscale | 4bpp packed grayscale | 4bpp packed grayscale |
| **Gray Levels** | 16 levels ($0=\text{black}$, $15=\text{white}$) | 16 levels ($0=\text{black}$, $15=\text{white}$) | 16 levels ($0=\text{black}$, $15=\text{white}$) |
| **Bytes Per Row** | $600 / 2 = 300$ bytes | $824 / 2 = 412$ bytes | $600 / 2 = 300$ bytes |
| **Framebuffer Size** | $300 \times 800 = 240,000$ bytes | $412 \times 1200 = 494,400$ bytes | $300 \times 800 = 240,000$ bytes |
| **Device Node** | `/dev/fb0` | `/dev/fb0` | `/dev/fb0` |
| **Refresh IOCTL** | `FBIO_EINK_UPDATE_DISPLAY_AREA` | `FBIO_EINK_UPDATE_DISPLAY_AREA` | `FBIO_EINK_UPDATE_DISPLAY_AREA` |

---

## 3. Linux Input Subsystem & Evdev Nodes

| Input Component | Device Node | Subsystem Type | Evdev Grabbing (`EVIOCGRAB`) |
| :--- | :--- | :--- | :--- |
| **Physical QWERTY Keyboard** | `/dev/input/event0` | Linux Evdev (`EV_KEY`) | Intercepted during hotkey sequences |
| **Five-Way Joystick Controller** | `/dev/input/event1` | Linux Evdev (`EV_KEY`) | Intercepted during hotkey sequences |
| **Volume & Power Sliders** | `/dev/input/event2` (K3) | Linux Evdev (`EV_KEY`) | Not intercepted by Launchpad |

---

## 4. Keypad Characteristics & Physical Layouts

### 4.1 Kindle 2 & Kindle Keyboard (K3) Layout
- **Keyboard Matrix**: 4 physical rows.
- **Numbers**: Typed via `Alt + <letter>` combinations (e.g. `Alt + Q = 1`, `Alt + W = 2`).
- **Modifiers**: Left Shift, Right Shift, Alt, Sym.
- **Navigation**: 5-way joystick (Up, Down, Left, Right, Select/Press). Left/Right page-turn buttons.

### 4.2 Kindle DX Layout
- **Keyboard Matrix**: 5 physical rows with a dedicated physical numeric top row (`1` through `0`).
- **Symbolics**: Numbers generate direct evdev codes `KEY_1` through `KEY_0` without `Alt` combination.

---

## 5. Virtual Filesystem Virtual Injection Nodes

| Path | Purpose | Format |
| :--- | :--- | :--- |
| `/proc/keypad` | Native framework key injection | ASCII character scancode or decimal |
| `/proc/fiveway` | Five-way controller injection | Directional code (`U`, `D`, `L`, `R`, `P`) |
| `/var/run/launchpad.pid` | Single-instance flock lockfile | ASCII process ID |
| `/mnt/us/launchpad/` | User storage root | Standard FAT32 / VFAT mount |
