# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Kindle Launchpad is a lightweight C daemon and hotkey launcher for Amazon Kindle e-ink devices featuring physical keyboards (Kindle Keyboard / K3, Kindle DX, Kindle 2). It intercepts hardware key events directly from the Linux input subsystem (`/dev/input/event*`) via `EVIOCGRAB`, matches user-defined key sequences in `.ini` files, executes actions (shell commands, framework keystrokes, or scripts), and draws visual feedback to the e-ink framebuffer (`/dev/fb0`).

## Build & Development Commands

### Building

The Makefile uses target names for specific build configurations. Note that GNU Make defaults to creating the output directory if no target is passed, so always specify the target explicitly (e.g. `make all`).

- **Native Host Build (Release)**:
  ```bash
  make all
  ```
  Produces stripped binary at `./bin/launchpad`.

- **Native Host Build (Debug)**:
  ```bash
  make BUILD_SPEC=Debug all
  ```
  Produces unstripped binary with `-O0 -g3` at `./bindebug/launchpad`.

- **Kindle ARMv6 Cross-Compilation (Release)**:
  ```bash
  make kindle
  # Equivalent to: make BUILD_SPEC=Kindle CROSS_COMPILE=arm-linux-gnueabi- all
  ```
  Produces stripped ARMv6 binary at `./binarm/launchpad` using `arm-linux-gnueabi-gcc`.

- **Kindle ARM Cross-Compilation (Debug)**:
  ```bash
  make BUILD_SPEC=armDebug CROSS_COMPILE=arm-linux-gnueabi- all
  ```
  Produces debug ARM binary at `./armdebug/launchpad`.

- **Clean Build Artifacts**:
  ```bash
  make clean
  ```
  Removes `bin/`, `binarm/`, `bindebug/`, and `armdebug/`.

### Running & Debugging

- **Foreground Run (No Daemonization)**:
  ```bash
  ./bin/launchpad -d
  ```
- **Custom Event Device Paths**:
  ```bash
  ./bin/launchpad -d -kb/dev/input/event0 -fw/dev/input/event1
  ```
- **Diagnostic Logs**: When running without an interactive terminal, output is redirected to `launchpad.log` in the executable's directory.

### Packaging

- Release packages are Kindle OTA update `.bin` archives in `packages/`.
- Packaging scripts reside in `packaging/src/` (`build-updates.sh`, `install.sh`, `uninstall.sh`).

## Architecture & Code Structure

### 1. Daemon Lifecycle & Input Polling (`src/main.c`)
- **Initialization**: Determines runtime path via `/proc/self/exe`, parses flags (`-d`, `-kb`, `-fw`), and forks a background daemon unless `-d` is passed.
- **Signal Handling**: Catches `SIGINT`, `SIGTERM`, and `SIGHUP`. `SIGHUP` triggers full configuration reinitialization (`reinit`).
- **Device Polling**: Opens keyboard (`/dev/input/event0`), five-way controller (`/dev/input/event1`), and volume keys (`/dev/input/event2` on K3). Uses `poll()` to multiplex input events.
- **Input Grabbing**: When the hotkey sequence starts (Introducer key pressed), calls `ioctl(fd, EVIOCGRAB, 1)` to exclusively lock input events from the Kindle OS. If the sequence finishes or times out (`HotInterval`), releases grab via `ioctl(fd, EVIOCGRAB, 0)`.

### 2. Hotkey Dispatcher & Configuration (`src/launchpad.c`, `src/inifile.c`)
- **Config Aggregation**: On startup or reload (`Shift Shift Space`), scans the executable's directory for all `*.ini` files (`launchpad.ini`, `servicecmds.ini`, `fbreader.ini`, etc.) and registers key sequence bindings into an action table.
- **Action Execution (`execute_action`)**:
  - `!<command>`: Executes a system shell command via `system()`.
  - `@<script>`: Runs a Kindle Framework script from `scripts/` (matches hotkeys package format).
  - `#<keys>`: Emulates Kindle Framework search-bar keystrokes.
  - Quoted string / raw tokens: Injects simulated keypress events into the Linux input subsystem.
  - Internal Triple-Shift: Calls `do_screenshot()` directly.

### 3. E-Ink Framebuffer & Status Bar (`src/screen.c`, `src/statusbar.c`)
- Directly opens `/dev/fb0`.
- Renders an overlay status bar in the bottom corner using an embedded 12x22 bitmap font (`include/fnt12x22.h`) to show action progress (`^[...]`), `Success!`, or `Failure.`.
- Flashes updates using Kindle-specific e-ink ioctl calls (`FBIO_EINK_UPDATE_DISPLAY`).

### 4. Framebuffer Capture (`src/screenshot.c`)
- Reads raw pixel data from `/dev/fb0` and outputs standard 4bpp/8bpp uncompressed BMP files to `/mnt/us/screenshot.bmp`.

### 5. Remote Command Entry (`src/rce.c`)
- Optional listener running in a dedicated `pthread` that binds a multicast UDP socket to accept remote trigger messages over WiFi or usbnet.

### 6. Hardware Key Mapping (`src/keydefs.c`, `src/asciitab.c`)
- Maps Linux kernel input event codes to symbolic key names (`KPKEY_*`) and translates ASCII characters into Kindle key injection event sequences.

## Environment & Toolchain Conventions

- **Target Architecture**: ARMv6 (Freescale i.MX31 / i.MX35), Linux 2.6.x kernel.
- **Toolchain**: `arm-linux-gnueabi-gcc` with `-msoft-float` and `-lpthread`.
- **Target Installation Path**: `/mnt/us/launchpad/` on device (USB user storage root).
