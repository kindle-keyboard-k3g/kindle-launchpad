# Kindle Keyboard (Kindle 3) Hardware Profile

This document details the hardware specifications, kernel interfaces, device nodes, and input subsystem behavior for the **Amazon Kindle Keyboard** (Kindle 3 / K3 family).

---

## 1. Device Variants & Identifiers

| Model | Market Name | Part / Serial Prefix | Wireless Capabilities |
| :--- | :--- | :--- | :--- |
| **K3G** | Kindle Keyboard 3G (US/Canada) | `B006` | Wi-Fi (802.11b/g/n) + Cellular (AT&T 3G) |
| **K3W** | Kindle Keyboard Wi-Fi | `B008` | Wi-Fi only (802.11b/g/n) |
| **K3GB** | Kindle Keyboard 3G (Europe/UK) | `B00A` | Wi-Fi (802.11b/g/n) + Cellular (Vodafone 3G) |

---

## 2. Hardware Specifications

- **SoC**: Freescale i.MX353 Multimedia Applications Processor
  - Core: ARM1136JF-S (ARMv6 architecture) @ 532 MHz
  - Instruction Set: ARMv6KZ with VFP11 FPU (soft-float ABI used for compatibility)
- **Memory**: 256 MB Mobile DDR SDRAM @ 133 MHz
- **Storage**: 4 GB eMMC NAND flash
  - Rootfs: `/` (ext3, read-only in normal operation)
  - User Storage: `/mnt/us` (FAT32, USB mass storage mount)
  - Configuration Path: `/mnt/us/launchpad/`
- **Display**: 6.0-inch E-Ink Pearl display
  - Resolution: 600 × 800 pixels
  - Density: 167 DPI
  - Color Depth: 4bpp (16 levels of grayscale), packed as 2 pixels per byte (4 bits each)
- **Kernel**: Linux 2.6.26-rt-lab126 custom kernel for Amazon Lab126

---

## 3. Linux Input Subsystem & Device Nodes

The Kindle Keyboard exposes physical input peripherals via the standard Linux `evdev` event interface in `/dev/input/`:

| Device Node | Kernel Device Name | Subsystem / Hardware |
| :--- | :--- | :--- |
| `/dev/input/event0` | `mx35_kpp` / `matrix-keypad` | Physical 40-key QWERTY keyboard matrix |
| `/dev/input/event1` | `fiveway` | 5-way directional joystick and center select button |
| `/dev/input/event2` | `volume` | Dedicated volume rocker (+ and - buttons) |

### 3.1 Kernel Input Grabbing (`EVIOCGRAB`)
Kindle Launchpad uses the Linux `ioctl(fd, EVIOCGRAB, 1)` mechanism on `/dev/input/event0` and `/dev/input/event1`:
- When grabbed, the Kindle framework Java application (`cvm` / WebReader) receives **no key events**.
- When released via `ioctl(fd, EVIOCGRAB, 0)`, key events resume flowing to native applications.

---

## 4. Kindle 3 Keycode Discrepancies & Mapping

Because Amazon updated the keypad matrix in the K3 compared to the earlier Kindle 2 and Kindle DX, several scancodes differ:

| Symbolic Key | K2 / DX Scancode | K3 Scancode (`key_codes::K3KEY_*`) | Canonical Substitution |
| :--- | :--- | :--- | :--- |
| **5-Way Press** | 92 (`KPKEY_FW_PRESS`) | 194 (`K3KEY_FW_PRESS`) | 92 |
| **5-Way Up** | 122 (`KPKEY_FW_UP`) | 103 (`K3KEY_FW_UP`) | 122 |
| **5-Way Down** | 123 (`KPKEY_FW_DOWN`) | 108 (`K3KEY_FW_DOWN`) | 123 |
| **Symbol (`Sym`)** | 94 (`KPKEY_SYM`) | 126 (`K3KEY_SYM`) | 94 |
| **Font Size (`Aa`)**| 90 (`KPKEY_AA`) | 190 (`K3KEY_AA`) | 90 |
| **Home** | 98 (`KPKEY_HOME`) | 102 (`K3KEY_HOME`) | 98 |
| **Back** | 91 (`KPKEY_BACK`) | 158 (`K3KEY_BACK`) | 91 |
| **Page Forward** | 124 (`KPKEY_PGFWD`) | 191 (`K3KEY_PGFWD`) | 124 |
| **Left Page Back** | 109 (`KPKEY_PGBCK`) | 193 (`K3KEY_LPGBCK`) | 109 |
| **Left Page Fwd** | 124 (`KPKEY_PGFWD`) | 104 (`K3KEY_LPGFWD`) | 124 |

`launchpad::domain::KeyTable::get_substitute_code()` transparently normalizes all K3 scancodes to their canonical equivalents, allowing users to write universal INI configurations across all Kindle models.

---

## 5. Kernel Virtual Keypad Injection (`/proc`)

To simulate keystrokes without injecting synthetic `evdev` events into grabbed nodes, Kindle Launchpad communicates with the Amazon kernel driver interface:

- **`/proc/keypad`**: Accepts ASCII key names or scancode integers:
  ```bash
  echo "28" > /proc/keypad   # Injects Enter
  ```
- **`/proc/fiveway`**: Accepts directional joystick commands (`up`, `down`, `left`, `right`, `select`).

---

## 6. E-Ink Framebuffer Interface

- **Node**: `/dev/fb0`
- **Memory**: Mapped into user-space via `mmap()` (total size: $600 \times 800 \times 0.5 = 240,000$ bytes).
- **Format**: 4bpp packed grayscale (2 pixels per byte, high nibble = pixel $2n$, low nibble = pixel $2n+1$).
- **Update IOCTL**: `FBIO_EINK_UPDATE_DISPLAY_AREA` updates arbitrary bounding boxes with partial or full waveform flashing.
