# Kindle 2 & K2 International Hardware Profile

This document details the hardware specifications, kernel interfaces, device nodes, and input subsystem behavior for the **Amazon Kindle 2** (K2 family).

---

## 1. Device Variants & Identifiers

| Model | Market Name | Serial Prefix | Wireless Capabilities | Internal Storage |
| :--- | :--- | :--- | :--- | :--- |
| **K2** | Kindle 2 (US) | `B002` | Cellular (Sprint CDMA / EV-DO) | 2 GB (1.4 GB accessible) |
| **K2i** | Kindle 2 International | `B003` | Cellular (AT&T / Vodafone 3G GSM) | 2 GB (1.4 GB accessible) |

---

## 2. Hardware Specifications

- **SoC**: Freescale i.MX31 Multimedia Applications Processor
  - Core: ARM1136JF-S (ARMv6 architecture) @ 532 MHz
  - Instruction Set: ARMv6KZ (soft-float ABI)
- **Memory**: 32 MB or 64 MB Mobile DDR SDRAM (depending on revision)
- **Storage**: 2 GB SanDisk / Samsung NAND flash
  - User Storage: `/mnt/us` (FAT32, ~1.4 GB usable)
  - Configuration Path: `/mnt/us/launchpad/`
- **Display**: 6.0-inch E-Ink Vizplex display
  - Resolution: 600 × 800 pixels
  - Density: 167 DPI
  - Color Depth: 4bpp (16 levels of gray)
  - Framebuffer Memory Size: $600 \times 800 \times 0.5 = 240,000$ bytes
- **Kernel**: Linux 2.6.22-lab126 custom kernel

---

## 3. Physical Keyboard Matrix

The Kindle 2 keyboard layout is nearly identical to the Kindle DX:
- Features a dedicated top numeric row (`1` through `0`).
- Shares the canonical scancodes (`KPKEY_*`) with the Kindle DX.
- Uses the standard 5-way joystick for navigation (`event1`).

---

## 4. Input Subsystem Device Nodes

| Device Node | Kernel Device Name | Subsystem / Hardware |
| :--- | :--- | :--- |
| `/dev/input/event0` | `mx31_kpp` | Physical 46-key QWERTY keyboard matrix |
| `/dev/input/event1` | `fiveway` | 5-way directional joystick and select button |
| `/dev/input/event2` | `volume` | Volume rocker (+ and - buttons) |

---

## 5. Memory Footprint Considerations

Because the Kindle 2 has only 32 MB or 64 MB of total system RAM (with the Java framework consuming over 70% of available memory), background processes must maintain an exceptionally small RSS (Resident Set Size).

Kindle Launchpad's modern C++20 architecture achieves:
- **Total Static Binary Size**: ~180 KB stripped.
- **Runtime Memory Consumption (RSS)**: < 1.2 MB.
- **Zero Heap Churn**: Fixed-size buffers, `std::string_view` passes, and pre-allocated lookup tables ensure the daemon never triggers Linux OOM killer invocations.
