# Changelog

All notable changes to **Kindle Launchpad** are documented in this file.

The primary project thread and release announcements are hosted on MobileRead:
[MobileRead Thread #97636 - launchpad -- yet another hotkey manager for Kindle](https://www.mobileread.com/forums/showthread.php?t=97636)

---

## [0.0.1d] - 2015-09-11
### Packaging & Compatibility Update (by eschwartz)
- **Repackaged with modern KindleTool**: All update `.bin` packages (`lpad-pkg-001d.zip`) were rebuilt using the latest `kindletool` utility.
- **Fixed OTA target values**: Resolved the issue where legacy installers produced low target OTA values, which caused "Update Error: 3" or disabled ("greyed out") "Update Your Kindle" menu options on newer Kindle firmware releases (specifically 3.4.x on Kindle Keyboard / K3, K3G, and DX).
- **Latest Distribution**: This remains the latest and most stable release of Launchpad packages for Kindle 2, Kindle DX, and Kindle Keyboard 3.

---

## [0.0.1c] - 2011-02-02
### Added
- **Remote Command Entry (RCE)**: Added remote command entry listener via `rce.c` and configuration in `rce.ini`.
- Allows sending arbitrary commands over a network socket to trigger launchpad actions remotely.

---

## [0.0.1b] - 2011-01-29
### Added
- **Screenshot Capture**: Pressing `Shift` three times sequentially (`Shift Shift Shift`) captures the current framebuffer screen state and saves it as `screenshot.bmp` in the Kindle root storage directory (`/mnt/us/screenshot.bmp`).
- BMP writer implementation in `src/screenshot.c` and `include/screenshot.h`.

---

## [0.0.1a] - 2010-12-04
### Changed
- Standardized package versioning scheme to `0.0.1a` across all device-specific update bundles.

---

## [Unversioned Maintenance] - 2010-11-29
### Added
- Source code archive (`launchpad.tar.gz`) separated from binary package distributions.
- Additional internal stability improvements.

---

## [Unversioned Maintenance] - 2010-11-21
### Added
- **InterKeyDelay**: Added `InterKeyDelay` runtime configuration option to allow executing prolonged Kindle-specific framework macros and sequences.

---

## [Unversioned - Kindle 3 Support] - 2010-10-14
### Added
- Extended hardware support and key mappings for **Kindle 3 (Kindle Keyboard)** devices (`k3w`, `k3g`, `k3gb`).
- Successfully tested on Kindle DX Graphite and Kindle 3 WiFi / 3G.
- *Note*: Specific K3 special symbols (GBp, Euro, open/close quotes, inverted `!` and `?`) are not supported due to framework search box differences.

---

## [Unversioned Refresh] - 2010-09-09
### Changed
- Refreshed `launchpad.tar.gz` with various bug fixes.
- Enhanced default hotkey layout in `launchpad.ini`.
- Improved multi-file `.ini` configuration merging logic.

---

## [Initial Release] - August 2010
### Added
- Initial release by **Andy M. (h1uke)** for Kindle DX Graphite (`dxg`).
- Low-level keyboard and 5-way joystick event scanner running transparently in the background (`/dev/input/event*`).
- Intercepts configurable Introducer key (default `Shift`) and Trailer key (`Space` or timeout).
- INI-based configuration parser with multi-file support (`launchpad.ini`, `servicecmds.ini`, `fbreader.ini`).
- Supports 4 action execution modes:
  - `!` (shell command execution)
  - `@` (framework script emulation)
  - `#` (Kindle framework key sequence)
  - Simulated key sequences / text strings
- Built-in status feedback via Kindle status bar, flashing screen, or LEDs.
- Hot-reload mechanism via `kill -HUP $(pidof launchpad)` or `Shift Shift Space`.
- Custom update system (`Shift Shift I`) executing scripts from `/mnt/us/customupdates`.
