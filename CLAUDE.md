# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Kindle Launchpad is a lightweight C daemon and hotkey launcher for Amazon Kindle e-ink devices featuring physical keyboards (Kindle Keyboard / K3, Kindle DX, Kindle 2). It intercepts hardware key events directly from the Linux input subsystem (`/dev/input/event*`) via `EVIOCGRAB`, matches user-defined key sequences in `.ini` files, executes actions (shell commands, framework keystrokes, or scripts), and draws visual feedback to the e-ink framebuffer (`/dev/fb0`).

## Build & Development Commands

### Building with CMake (Recommended)

Kindle Launchpad uses a modern C++20 build system managed by CMake and standardized presets in `CMakePresets.json`:

- **Host Debug Build & Tests**:
  ```bash
  cmake --preset host-debug
  cmake --build --preset host-debug
  ctest --preset host-test
  # Or run tests directly: ./build/host-debug/unit_tests
  ```

- **Host Release Build**:
  ```bash
  cmake --preset host-release
  cmake --build --preset host-release
  ```

- **Kindle ARMv6 Cross-Compilation (Release)**:
  ```bash
  # Uses arm-linux-gnueabi-g++ via cmake/Toolchain-Kindle-ARMv6.cmake
  cmake --preset kindle-k3-release
  cmake --build --preset kindle-k3-release
  ```
  Produces stripped ARMv6 binary at `./build/kindle-k3-release/launchpad`.

- **Staging & Packaging**:
  ```bash
  # Stage binary and configuration files into staging/src/launchpad/
  cmake --build --preset kindle-k3-release --target package-stage

  # Build Kindle OTA update packages (requires kindletool in PATH)
  cmake --build --preset kindle-k3-release --target package-ota
  ```

### Building with Legacy Makefile (Alternative)

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

- **Clean Build Artifacts**:
  ```bash
  make clean
  ```

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

Kindle Launchpad is structured as a modular C++20 application adhering to SOLID principles and Object Calisthenics:

### 1. Application Lifecycle & Composition Root (`src/main.cpp`, `launchpad/system/application.hpp`)
- **Initialization**: Determines runtime path via `/proc/self/exe`, parses flags (`-d`, `-kb`, `-fw`), and runs as daemon unless `-d` is passed.
- **Dependency Injection**: Instantiates concrete HAL services (`EvdevInputSource`, `KindleDisplay`, `LinuxCommandRunner`, `LinuxKeyInjector`) and injects them into `Application`.
- **Signal Handling**: Async-signal-safe flag management for `SIGINT`, `SIGTERM`, and `SIGHUP` (dynamic configuration reload).
- **PID Locking**: `PidLock` manages `/var/run/launchpad.pid` using RAII and non-blocking `flock(LOCK_EX | LOCK_NB)`.

### 2. Configuration & Hotkey Dispatcher (`launchpad/config/`, `launchpad/domain/`)
- **Config Aggregation (`config::ConfigScanner`)**: Scans directory for all `*.ini` files (`launchpad.ini`, `servicecmds.ini`, `fbreader.ini`, etc.) and registers bindings into `domain::ActionRegistry`.
- **Hotkey State Machine (`domain::SequenceMatcher`)**: Tracks sequence transitions across Introducer (`Shift`), key codes, `HotInterval` timeouts, and Trailer (`Enter`).
- **Action Execution (`domain::ActionExecutor`)**:
  - `!<command>`: Executes system shell command via `ICommandRunner`.
  - `@<script>`: Runs Kindle Framework script from `scripts/`.
  - `#<keys>`: Emulates Kindle Framework search-bar keystrokes via `IKeyInjector`.
  - Quoted string / raw tokens: Injects simulated keypress events via `IKeyInjector`.
  - Internal Triple-Shift: Calls `ScreenshotAction` directly.

### 3. Hardware Abstraction Layer (HAL) (`launchpad/hal/`)
- `IInputSource`: Abstract interface declaring `poll_events()`, `grab()`, and `release()`. Concrete `EvdevInputSource` polls `/dev/input/event*` and controls `ioctl(EVIOCGRAB)`.
- `IDisplay`: Abstract interface declaring framebuffer dimensions and update rects. Concrete `KindleDisplay` handles `/dev/fb0` mmap and `FBIO_EINK_UPDATE_DISPLAY` ioctls.
- `ICommandRunner`: Interface for executing system commands (`LinuxCommandRunner` vs `MockCommandRunner`).
- `IKeyInjector`: Interface for injecting simulated keystrokes (`LinuxKeyInjector` writes to `/proc/keypad` and `/proc/fiveway`).

### 4. E-Ink Framebuffer & Status Bar (`launchpad/ui/`)
- `ui::FontRenderer`: Renders 12x22 bitmap glyphs into 4bpp/8bpp pixel buffers.
- `ui::StatusBar`: Renders bottom-corner feedback (`^[...]`, `Success!`, `Failure.`) with background save and restore.
- `ui::ScreenshotWriter`: Exports framebuffer to uncompressed BMP format.

### 5. Remote Command Entry (`launchpad/system/rce_server.hpp`)
- `RceServer`: UDP multicast listener bound to 224.0.0.1:4444 to accept remote trigger messages over WiFi or usbnet.

## Environment & Toolchain Conventions

- **Target Architecture**: ARMv6 (Freescale i.MX31 / i.MX35), Linux 2.6.x kernel.
- **Toolchain**: `arm-linux-gnueabi-gcc` with `-msoft-float` and `-lpthread`.
- **Target Installation Path**: `/mnt/us/launchpad/` on device (USB user storage root).

## Coding Standards & Development Rules

All code changes and additions must strictly adhere to the project standards documented in `docs/README.md` and `docs/sot/coding-standards.md`:

### 1. Test-Driven Development (TDD) & `/tdd`
- Follow Red-Green-Refactor: write failing unit/integration tests before writing implementation code.
- Invoke the `/tdd` skill when developing features or fixing bugs.
- Decouple pure logic (parsing, mapping, state transitions) from low-level Linux hardware I/O (`/dev/input/*`, `/dev/fb0`) for fast host-based test execution.

### 2. SOLID Principles in Modular C
- **Single Responsibility (SRP)**: Each function and `.c` module must do one thing with one reason to change.
- **Open/Closed (OCP)**: Extend functionality through lookup tables, configuration, and function pointers rather than editing monolithic switch/if-else ladders.
- **Liskov Substitution (LSP)**: Interchangeable display/input drivers and abstractions must preserve behavioral contracts.
- **Interface Segregation (ISP)**: Header files (`.h`) must declare only what callers require. Internal helpers remain `static`.
- **Dependency Inversion (DIP)**: Depend on abstractions rather than hardcoded hardware devices. Inject file descriptors and device paths.

### 3. Object Calisthenics (Systems C)
1. **One indent level per function**: Extract nested logic into named helper functions.
2. **No `else`**: Use guard clauses, early returns, or lookup structures.
3. **Wrap domain primitives**: Wrap raw scalars in dedicated typedef structs.
4. **First-class collections**: Encapsulate lists and tables in dedicated structs with focused accessor functions.
5. **One dot/arrow per line**: Avoid deep pointer dereferencing chains (`a->b->c`).
6. **No abbreviations**: Use clear, descriptive names (avoid `buf`, `tmp`, `fn`, `pact`).
7. **Keep entities small**: Functions ≤ 15 lines, structs ≤ 100 lines, modules < 200 lines.
8. **≤ 2 instance variables per entity**: Decompose structs into focused sub-structures.
9. **Tell, don't ask**: Expose behaviors through functions rather than leaking struct internals.

### 4. Fail-Fast & Defensive Programming
- Validate inputs, buffers, and state at boundaries immediately.
- Never swallow errors silently; check all system call return values (`read`, `write`, `ioctl`, `system`).
- Ensure all resources (descriptors, locks, grabbed devices) are cleaned up in error paths.

