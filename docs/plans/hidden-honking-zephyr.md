# Implementation Plan: Modern C++20 Refactoring for Kindle Launchpad

## Context & Motivation

**Kindle Launchpad** has historically run on a legacy C codebase written in 2010. While functional on early Kindle devices (Kindle 2, DX, Kindle Keyboard 3), the codebase suffers from:
- Monolithic procedural code with global mutable state (`pactions`, `hot_sequence`, `fd_kbd`, `fd_fw`).
- Raw pointer arithmetic, unchecked `sprintf`/`strcpy` buffers, and lack of RAII for Linux input grabbing (`ioctl(..., EVIOCGRAB)`), framebuffer mappings, and file descriptors.
- Zero automated testing or CI validation due to tight coupling between business logic and Linux hardware device nodes (`/dev/fb0`, `/dev/input/event*`).
- Inflexible build scripts (legacy GNU Makefile) without modern preset support, sanitizer integration, or standardized toolchains.

Following modern sister projects **`kindle-myts`** and **`papergram`**, and integrating with **`kindle-sdk`**, this plan defines the complete refactoring of Kindle Launchpad into a **modern C++20 architecture** strictly adhering to **TDD**, **SOLID**, and **Object Calisthenics**.

---

## Architectural Principles & Constraints

1. **C++20 Standard**: Modern language features (`std::span`, `std::string_view`, `std::variant`, `std::optional`, strongly typed enums, concepts, RAII).
2. **SOLID Architecture**:
   - **SRP**: Strict division into `core/`, `config/`, `domain/`, `hal/`, `ui/`, and `system/`.
   - **DIP**: Business logic and state machines depend on abstract interfaces (`IInputSource`, `IDisplay`), not concrete hardware device nodes.
3. **Object Calisthenics**:
   - 1 indentation level per function.
   - No `else` (early returns, guard clauses, pattern matching with `std::visit`).
   - Wrap domain primitives (`Milliseconds`, `KeyCode`, `KeySequence`).
   - First-class collections (`ActionRegistry`, `KeyMapTable`).
   - One dot/arrow per line.
   - No abbreviations (`ActionRegistry` not `pact`, `FileDescriptor` not `fd`).
   - Small entities (functions ≤15 lines, classes ≤100 lines, modules <200 lines, ≤2 instance variables via composition).
   - Tell, don't ask.
4. **Test-Driven Development (TDD & `/tdd`)**: Catch2 v3 test suite executing 100% natively on host without Kindle hardware.
5. **Full Backward Compatibility**: Unchanged support for existing `.ini` syntax (`[Settings]`, `[Actions]`, prefixes `!`, `@`, `#`, emulated keys), default shortcuts (`Shift Shift Space`, `Shift Shift Shift` screenshot), and Kindle models (K2, K2i, DX, DXi, DXG, K3G, K3W, K3GB).

---

## Target Directory Structure

```
kindle-launchpad/
├── CMakeLists.txt                      # Root modern CMake configuration
├── CMakePresets.json                   # Presets: host-debug, host-test, kindle-k3-release
├── cmake/
│   ├── CompilerWarnings.cmake          # Strict compiler warnings
│   ├── KindlePackaging.cmake           # kindletool integration for OTA packages
│   └── Toolchain-Kindle-ARMv6.cmake    # Wraps kindle-sdk/native/cmake/KindleArmv6.cmake
├── include/launchpad/
│   ├── core/
│   │   ├── result.hpp                  # Expected/Result<T, E> error handling
│   │   ├── units.hpp                   # Milliseconds, KeyCode, KeySequence value objects
│   │   ├── file_descriptor.hpp         # RAII ScopedFileDescriptor
│   │   └── logger.hpp                  # Diagnostic logger (console / launchpad.log)
│   ├── config/
│   │   ├── ini_parser.hpp              # Typesafe, pure-logic INI file parser
│   │   ├── settings.hpp                # Strongly-typed [Settings] model
│   │   └── config_scanner.hpp          # Multi-INI directory aggregator
│   ├── domain/
│   │   ├── action.hpp                  # Action variant (Shell, Script, Keys, Screenshot)
│   │   ├── action_registry.hpp         # First-class collection wrapping actions
│   │   ├── key_definition.hpp          # Linux evdev to Kindle keycode lookup table
│   │   ├── ascii_translator.hpp        # ASCII character to Kindle keystroke mapper
│   │   ├── sequence_matcher.hpp        # Hotkey state machine (Introducer, Trailer, HotInterval)
│   │   └── action_executor.hpp         # Dispatches matched actions
│   ├── hal/
│   │   ├── input_source.hpp            # IInputSource interface + ScopedInputGrab
│   │   ├── evdev_input_source.hpp      # Linux /dev/input/event* implementation with EVIOCGRAB
│   │   ├── mock_input_source.hpp       # Mock input source for unit/integration tests
│   │   ├── display.hpp                 # IDisplay interface (wraps kindle-sdk EinkDisplay)
│   │   ├── kindle_display.hpp          # Concrete /dev/fb0 mmap + e-ink ioctl updater
│   │   └── mock_display.hpp            # In-memory mock framebuffer for host testing
│   ├── ui/
│   │   ├── font.hpp                    # 12x22 bitmap font renderer
│   │   ├── canvas.hpp                  # 4bpp/8bpp grayscale canvas operations
│   │   ├── status_bar.hpp              # Bottom-right visual feedback widget
│   │   └── screenshot_writer.hpp       # Framebuffer BMP file exporter
│   └── system/
│       ├── daemon.hpp                  # Linux daemonization & lockfile manager
│       ├── signal_handler.hpp          # Signal dispatcher (SIGINT, SIGTERM, SIGHUP)
│       └── rce_server.hpp              # Multicast UDP Remote Command Entry server
├── src/                                # Concrete C++20 implementations matching include/
│   ├── core/
│   ├── config/
│   ├── domain/
│   ├── hal/
│   ├── ui/
│   ├── system/
│   └── main.cpp                        # Entry point wiring components via Dependency Injection
├── tests/
│   ├── unit/                           # Unit tests for core, config, and domain
│   ├── mocks/                          # Mock implementations of HAL
│   └── integration/                    # End-to-end simulated hotkey execution tests
└── packages/                           # Preserved release packages & build targets
```

---

## Phased Implementation Roadmap

### Phase 1: Build Infrastructure & Test Harness (Day 1)
- **Files Created**:
  - `CMakeLists.txt`: Root project setup targeting C++20, warnings, Catch2 v3 integration via FetchContent (with local fallback).
  - `CMakePresets.json`: `host-debug`, `host-release`, `host-test`, `kindle-k3-release`.
  - `cmake/CompilerWarnings.cmake`: `-Wall -Wextra -Wpedantic -Wconversion -Wshadow`.
  - `cmake/Toolchain-Kindle-ARMv6.cmake`: References `/home/samuelcaldas/repos/kindle/kindle-sdk/native/cmake/KindleArmv6.cmake`.
- **Testing**:
  - Initial `tests/unit/test_smoke.cpp` with Catch2 v3 to verify host test runner passes.

### Phase 2: Core Primitives & Typesafe INI Parser (Day 1-2)
- **TDD Workflow**:
  - Write `tests/unit/test_ini_parser.cpp` covering section parsing (`[Settings]`, `[Actions]`), comment handling (`;`), whitespace trimming, case insensitivity, and invalid syntax rejection.
  - Write `tests/unit/test_units.cpp` covering `Milliseconds`, `KeyCode`, `KeySequence`.
- **Implementation**:
  - `include/launchpad/core/units.hpp`: Strongly typed value objects.
  - `include/launchpad/core/file_descriptor.hpp`: Move-only RAII `ScopedFileDescriptor`.
  - `include/launchpad/core/result.hpp`: `std::expected` / `Result<T, Error>` pattern.
  - `include/launchpad/config/ini_parser.hpp` & `src/config/ini_parser.cpp`: Pure C++20 parser returning parsed sections.
  - `include/launchpad/config/config_scanner.hpp` & `src/config/config_scanner.cpp`: Scans a directory for all `*.ini` files and aggregates settings/actions.

### Phase 3: Domain Layer (Key Mapping, Sequence Matcher & Action Registry) (Day 2)
- **TDD Workflow**:
  - Write `tests/unit/test_key_definition.cpp`: Verify evdev code to `KeyCode` and string symbolic names (`Shift`, `Enter`, `Space`, `0-9`, `A-Z`).
  - Write `tests/unit/test_ascii_translator.cpp`: Verify character-to-framework key script translation.
  - Write `tests/unit/test_sequence_matcher.cpp`: Test Introducer detection (`Shift`), key sequence buffering, `HotInterval` timeout detection, Trailer key handling (`Enter`), and sequence resets.
  - Write `tests/unit/test_action_registry.cpp`: Test registration of `!`, `@`, `#`, and raw key actions.
- **Implementation**:
  - `include/launchpad/domain/action.hpp`: `std::variant<ShellAction, ScriptAction, KeyAction, ScreenshotAction>`.
  - `include/launchpad/domain/action_registry.hpp` & `src/domain/action_registry.cpp`: First-class collection mapping `KeySequence` to `Action`.
  - `include/launchpad/domain/key_definition.hpp` & `src/domain/key_definition.cpp`: Ported and cleaned from `src/keydefs.c`.
  - `include/launchpad/domain/ascii_translator.hpp` & `src/domain/ascii_translator.cpp`: Ported and cleaned from `src/asciitab.c`.
  - `include/launchpad/domain/sequence_matcher.hpp` & `src/domain/sequence_matcher.cpp`: Pure state machine managing hotkey state transitions.
  - `include/launchpad/domain/action_executor.hpp` & `src/domain/action_executor.cpp`: Clean execution dispatcher.

### Phase 4: Hardware Abstraction Layer (HAL) & Mocks (Day 3)
- **Design**:
  - `IInputSource`: Abstract interface declaring `poll_events()`, `grab_input()`, `release_input()`.
  - `ScopedInputGrab`: RAII guard that grabs input on construct and guarantees release on destruct.
  - `EvdevInputSource`: Linux evdev implementation polling `/dev/input/event0,1,2` and using `ioctl(EVIOCGRAB)`.
  - `MockInputSource`: In-memory event injector for integration tests.
  - `IDisplay`: Abstract interface integrating with `kindle-sdk/native/include/kindle/eink.hpp` (`kindle::EinkDisplay`).
  - `KindleDisplay`: Concrete `/dev/fb0` mmap and `FBIO_EINK_UPDATE_DISPLAY` ioctl handler.
  - `MockDisplay`: In-memory pixel buffer for visual verification in tests.
- **Testing**:
  - `tests/mocks/mock_input_source.cpp`, `tests/mocks/mock_display.cpp`.
  - `tests/integration/test_hotkey_workflow.cpp`: Feeds synthetic key events into `MockInputSource`, verifies `SequenceMatcher` recognizes sequence and triggers `ActionExecutor`.

### Phase 5: UI & Visual Feedback (Day 3)
- **Implementation**:
  - `include/launchpad/ui/font.hpp`: Wraps the embedded 12x22 font data (`include/fnt12x22.h`) with a clean glyph rendering interface.
  - `include/launchpad/ui/canvas.hpp`: 4bpp/8bpp drawing operations, text blitting, and box rendering.
  - `include/launchpad/ui/status_bar.hpp`: Manages the bottom-right status bar lifecycle (`^[...]`, `Success!`, `Failure.`).
  - `include/launchpad/ui/screenshot_writer.hpp`: Clean BMP exporter writing `/dev/fb0` memory to `/mnt/us/screenshot.bmp`.
- **Testing**:
  - `tests/integration/test_status_bar_ui.cpp`: Verify status bar text renders into `MockDisplay` canvas correctly.

### Phase 6: System Layer, Daemon & Main Application (Day 4)
- **Implementation**:
  - `include/launchpad/system/daemon.hpp` & `src/system/daemon.cpp`: Manages `fork()`, setsid, PID lockfile writing, and terminal detachment.
  - `include/launchpad/system/signal_handler.hpp`: POSIX signal management (`SIGINT`, `SIGTERM`, `SIGHUP` for dynamic `.ini` reloading).
  - `include/launchpad/system/rce_server.hpp`: Optional UDP multicast listener running on background `std::jthread`.
  - `src/main.cpp`: Orchestrates initialization via dependency injection, parses CLI arguments (`-d`, `-kb`, `-fw`), and runs the main event loop.

### Phase 7: Packaging & Cross-Compilation Verification (Day 4)
- **Kindle Tool Integration**:
  - `cmake/KindlePackaging.cmake`: Custom CMake targets to generate signed `.bin` updates via `kindletool` for models `k2`, `k2i`, `dx`, `dxi`, `dxg`, `k3g`, `k3w`, `k3gb`.
- **Documentation & CI**:
  - Update `README.md`, `CLAUDE.md`, and `docs/README.md` with new CMake instructions and presets.

---

## Verification & Testing Plan

### 1. Automated Host Unit & Integration Tests
Run complete test suite on desktop host without needing hardware:
```bash
cmake --preset host-debug
cmake --build --preset host-debug
ctest --preset host-debug --output-on-failure
```
- **Coverage**:
  - 100% coverage of `IniParser` with diverse malformed/valid `.ini` samples.
  - Sequence matching edge cases: double-shift, timing expiration (`HotInterval`), interrupted sequences, trailer keys.
  - Correct command dispatching across shell commands, framework scripts, and simulated keystrokes.
  - Status bar rendering into `MockDisplay`.

### 2. ARMv6 Cross-Compilation Build
Verify cross-compilation using `kindle-sdk` toolchain:
```bash
cmake --preset kindle-k3-release
cmake --build --preset kindle-k3-release
```
- Verify resulting binary `bin/launchpad` is an ARMv6 ELF binary:
  ```bash
  file build-kindle-k3/bin/launchpad
  # Must output: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked ... stripped
  ```

### 3. OTA Update Package Generation
```bash
cmake --build --preset kindle-k3-release --target package-ota
```
- Verify valid `.bin` files are output in `build-kindle-k3/packages/` for all supported models.
