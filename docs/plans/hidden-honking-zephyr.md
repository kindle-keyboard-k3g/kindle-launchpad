# Implementation Plan: Comprehensive Documentation & Docstrings for Kindle Launchpad

## Context & Motivation

Following the successful refactoring of **Kindle Launchpad** from its legacy 2010 C codebase into a modern, modular **C++20 architecture** (with 100% host test coverage and ARMv6 cross-compilation verified), the repository requires production-grade documentation:

1. **Header Docstrings**: The 33 public C++20 headers under `include/launchpad/` currently have **0 Doxygen docstrings**. Every public class, struct, enum, function, method, value object, and interface requires concise, structured Doxygen documentation conforming to the project's engineering standards (`docs/sot/coding-standards.md` & global `CLAUDE.md`).
2. **Markdown Documentation**: The `docs/` directory currently lacks subsystem guides, hardware specifications, INI configuration references, developer onboarding guides, and Architecture Decision Records (ADRs) found in sister projects (`kindle-sdk`, `kindle-myts`, `papergram`).

This plan defines the staged delivery of complete Doxygen docstrings across all 33 public headers and a comprehensive, publication-ready markdown documentation suite in `docs/`.

---

## 1. Docstring Standards & Scope (33 Headers)

All public headers must adhere to the standard Doxygen comment format:
- **File Headers**: `@file`, `@brief`, and `@details` explaining the component's role in the architecture.
- **Functions & Methods**:
  - Purpose statement (one sentence for simple accessors; detailed block for boundary/complex methods).
  - `@param[in,out]` for each parameter explaining expected ranges and constraints.
  - `@return` describing return value and potential error states.
  - Exception/Error specification: Explicitly document `noexcept` or describe errors returned via `core::Result<T, Error>`.
  - `@note` for thread-safety, signal safety, or Kindle hardware quirks (e.g. `EVIOCGRAB`, 4bpp packed pixels).
- **Types & Enums**: Clear `@brief` explaining the domain semantics of each enum value and struct field.

### Target Headers Matrix (33 Files)

| Subsystem | Header Files | Target Entities |
| :--- | :--- | :--- |
| **`core/`** (4 files) | `result.hpp`<br>`units.hpp`<br>`file_descriptor.hpp`<br>`logger.hpp` | `Error`, `Result<T, E>`, `Milliseconds`, `RawKeyCode`, `KeySequence`, `DisplayBounds`, `ScopedFileDescriptor`, `Logger`, `LogLevel` |
| **`config/`** (3 files) | `ini_parser.hpp`<br>`settings.hpp`<br>`config_scanner.hpp` | `IniSection`, `IniDocument`, `IniParser`, `LaunchpadSettings`, `ConfigScanner`, `ScannedConfiguration`, `ActionEntry` |
| **`domain/`** (6 files) | `action.hpp`<br>`action_registry.hpp`<br>`key_definition.hpp`<br>`ascii_translator.hpp`<br>`sequence_matcher.hpp`<br>`action_executor.hpp` | `Action` variant (`ShellAction`, `ScriptAction`, `KindleKeysAction`, `SendKeysAction`, `ScreenshotAction`), `ActionParser`, `ActionRegistry`, `KeyTable`, `AsciiTranslator`, `SendMode`, `KeyEventType`, `MatcherState`, `SequenceMatcher`, `ActionExecutor` |
| **`hal/`** (11 files) | `input_source.hpp`<br>`display.hpp`<br>`command_runner.hpp`<br>`key_injector.hpp`<br>`evdev_input_source.hpp`<br>`kindle_display.hpp`<br>`linux_command_runner.hpp`<br>`linux_key_injector.hpp`<br>`mock_input_source.hpp`<br>`mock_display.hpp`<br>`mock_command_runner.hpp` | `IInputSource`, `ScopedInputGrab`, `IDisplay`, `ICommandRunner`, `IKeyInjector`, `EvdevInputSource`, `KindleDisplay`, `LinuxCommandRunner`, `LinuxKeyInjector`, `MockInputSource`, `MockDisplay`, `MockCommandRunner` |
| **`ui/`** (3 files) | `font.hpp`<br>`status_bar.hpp`<br>`screenshot_writer.hpp` | `FontRenderer`, `GlyphMetrics`, `StatusBar`, `ScreenshotWriter`, 4bpp grayscale rendering helpers |
| **`system/`** (6 files) | `cli_options.hpp`<br>`signal_handler.hpp`<br>`pid_lock.hpp`<br>`daemon.hpp`<br>`rce_server.hpp`<br>`application.hpp` | `CliOptions`, `SignalHandler`, `PidLock`, `Daemon`, `RceServer`, `RceConfig`, `Application`, `ApplicationServices` |

---

## 2. Markdown Documentation Architecture (`docs/`)

The documentation suite in `docs/` is organized into focused, modular domains matching `kindle-sdk` and `kindle-myts`:

```
docs/
├── README.md                           # Master documentation portal, index, and reading path
├── getting-started.md                  # Quickstart: prerequisites, building, running, config
├── architecture/
│   ├── overview.md                     # High-level architecture, composition root, data flow
│   ├── core.md                         # Result<T, Error>, RAII types, units, logger
│   ├── configuration.md                # INI scanning, precedence, dynamic SIGHUP reload
│   ├── domain.md                       # Key mappings, SequenceMatcher state machine, actions
│   ├── hal.md                          # Hardware Abstraction Layer, evdev, EVIOCGRAB, display
│   ├── ui.md                           # 12x22 font renderer, 4bpp status bar, BMP exporter
│   └── system.md                       # Daemon lifecycle, signals, PID lock, RCE server
├── hardware/
│   ├── kindle-k3.md                    # Kindle Keyboard (K3G, K3W, K3GB) specs & input nodes
│   ├── kindle-dx.md                    # Kindle DX & DX Graphite specs & input nodes
│   ├── kindle-k2.md                    # Kindle 2 & K2i specs & input nodes
│   ├── eink-framebuffer.md             # 4bpp packed pixel format, mmap, FBIO_EINK ioctls
│   └── kindle-ioctls.md                # Kernel ioctl reference and safety guidelines
├── reference/
│   ├── configuration-spec.md           # Authoritative [Settings] & [Actions] specification
│   ├── keycodes.md                     # Symbolic key table (KPKEY_*) to evdev and ASCII
│   └── cli.md                          # Command-line options reference (-d, -kb, -fw)
├── development/
│   ├── build-and-test.md               # CMake presets, compiler warnings, CTest execution
│   ├── contributing.md                 # TDD (/tdd), SOLID rules, Object Calisthenics
│   └── deployment-and-packaging.md     # OTA packages, kindletool, update scripts
├── adr/
│   ├── README.md                       # Architecture Decision Records index and status
│   ├── 0001-modern-cpp20-refactoring.md
│   ├── 0002-zero-exception-error-handling.md
│   ├── 0003-hardware-abstraction-layer-for-host-testing.md
│   ├── 0004-object-calisthenics-composition.md
│   ├── 0005-host-mocks-and-native-tests.md
│   └── 0006-backward-compatible-ini-actions.md
└── sot/
    ├── README.md                       # Source of Truth index
    ├── coding-standards.md             # Updated engineering standards (TDD, SOLID, C++20)
    ├── hardware-profile.md             # Device capability matrix (K2, DX, K3)
    └── config-schema.md                # Authoritative INI configuration schema
```

---

## 3. Phased Implementation Plan

### Phase 1: Core & Configuration Layers (Docstrings & Docs)
1. **Header Docstrings**:
   - `include/launchpad/core/result.hpp`: Document `Error`, `Result<T, E>`, monadic methods, error propagation.
   - `include/launchpad/core/units.hpp`: Document `Milliseconds`, `RawKeyCode`, `KeySequence`, `DisplayBounds`.
   - `include/launchpad/core/file_descriptor.hpp`: Document `ScopedFileDescriptor` move semantics and RAII cleanup.
   - `include/launchpad/core/logger.hpp`: Document `LogLevel`, `Logger` methods and thread-safety.
   - `include/launchpad/config/settings.hpp`: Document `LaunchpadSettings` and field defaults.
   - `include/launchpad/config/ini_parser.hpp`: Document `IniSection`, `IniDocument`, and `IniParser::parse`.
   - `include/launchpad/config/config_scanner.hpp`: Document `ConfigScanner::scan_directory` and aggregation rules.
2. **Markdown Guides**:
   - `docs/getting-started.md`: Prerequisites, quick host build, running foreground, creating first hotkey.
   - `docs/architecture/overview.md`: Architectural map, composition root, data flow, component interactions.
   - `docs/architecture/core.md`: Error model (`Result<T, Error>`), RAII wrappers, strongly-typed domain units.
   - `docs/architecture/configuration.md`: INI scanning, directory aggregation, settings overriding, reload lifecycle.

### Phase 2: Domain Layer & Hardware Profiles
1. **Header Docstrings**:
   - `include/launchpad/domain/action.hpp`: Document `Action` variant, action data structs, `ActionParser`.
   - `include/launchpad/domain/action_registry.hpp`: Document `ActionRegistry` collection operations.
   - `include/launchpad/domain/key_definition.hpp`: Document `KeyTable`, evdev mappings, symbolic name lookup.
   - `include/launchpad/domain/ascii_translator.hpp`: Document `AsciiTranslator`, `SendMode`, key translations.
   - `include/launchpad/domain/sequence_matcher.hpp`: Document `MatcherState`, `KeyEventType`, `MatcherResult`, `SequenceMatcher` transitions and timeouts.
   - `include/launchpad/domain/action_executor.hpp`: Document `ActionExecutor::execute` and command dispatching.
2. **Markdown Guides**:
   - `docs/architecture/domain.md`: Hotkey state machine, sequence matching, introducer/trailer semantics, action dispatch.
   - `docs/hardware/kindle-k3.md`: Kindle Keyboard 3 hardware profile (SoC, RAM, input device nodes, display).
   - `docs/hardware/kindle-dx.md`: Kindle DX & DX Graphite profile.
   - `docs/hardware/kindle-k2.md`: Kindle 2 & K2 International profile.
   - `docs/reference/configuration-spec.md`: Complete `.ini` reference for `[Settings]` and `[Actions]`.
   - `docs/reference/keycodes.md`: Symbolic key names table (`KPKEY_*`), ASCII mapping, evdev codes.

### Phase 3: Hardware Abstraction Layer (HAL) & E-Ink Subsystem
1. **Header Docstrings**:
   - `include/launchpad/hal/input_source.hpp`: Document `IInputSource`, `InputKeyEvent`, `ScopedInputGrab`.
   - `include/launchpad/hal/display.hpp`: Document `IDisplay` interface and coordinate contracts.
   - `include/launchpad/hal/command_runner.hpp`: Document `ICommandRunner` interface.
   - `include/launchpad/hal/key_injector.hpp`: Document `IKeyInjector` interface.
   - `include/launchpad/hal/evdev_input_source.hpp`: Document `EvdevInputSource`, `ioctl(EVIOCGRAB)`, polling loop.
   - `include/launchpad/hal/kindle_display.hpp`: Document `KindleDisplay`, `/dev/fb0` mmap, e-ink ioctl updates.
   - `include/launchpad/hal/linux_command_runner.hpp`: Document `LinuxCommandRunner`.
   - `include/launchpad/hal/linux_key_injector.hpp`: Document `LinuxKeyInjector`, `/proc/keypad`, `/proc/fiveway`.
   - `include/launchpad/hal/mock_input_source.hpp`: Document `MockInputSource` test double.
   - `include/launchpad/hal/mock_display.hpp`: Document `MockDisplay` memory buffer.
   - `include/launchpad/hal/mock_command_runner.hpp`: Document `MockCommandRunner` verification helper.
2. **Markdown Guides**:
   - `docs/architecture/hal.md`: HAL design, dependency inversion, input grabbing mechanics, mock injection.
   - `docs/hardware/eink-framebuffer.md`: 4bpp packed pixel format (2 pixels per byte), mmap, e-ink flashing ioctls.
   - `docs/hardware/kindle-ioctls.md`: IOCTL reference (`FBIO_EINK_UPDATE_DISPLAY_AREA`), safety and waveform flags.

### Phase 4: UI, System Layer & Developer Guides
1. **Header Docstrings**:
   - `include/launchpad/ui/font.hpp`: Document `FontRenderer`, 12x22 bitmap glyph rendering.
   - `include/launchpad/ui/status_bar.hpp`: Document `StatusBar`, background save/restore, flashing triggers.
   - `include/launchpad/ui/screenshot_writer.hpp`: Document `ScreenshotWriter`, BMP file header generation.
   - `include/launchpad/system/cli_options.hpp`: Document `CliOptions::parse` and flags (`-d`, `-kb`, `-fw`).
   - `include/launchpad/system/signal_handler.hpp`: Document `SignalHandler`, async-signal-safe atomic flags.
   - `include/launchpad/system/pid_lock.hpp`: Document `PidLock`, non-blocking `flock(LOCK_EX | LOCK_NB)`.
   - `include/launchpad/system/daemon.hpp`: Document `Daemon::daemonize` and stdio redirection.
   - `include/launchpad/system/rce_server.hpp`: Document `RceServer`, multicast UDP listener.
   - `include/launchpad/system/application.hpp`: Document `ApplicationServices`, `Application` event loop.
2. **Markdown Guides**:
   - `docs/architecture/ui.md`: UI rendering, font rasterizer, status bar overlay, framebuffer capture.
   - `docs/architecture/system.md`: Daemonization, PID lockfile, POSIX signal dispatch, main event loop.
   - `docs/reference/cli.md`: Complete CLI flag reference and runtime behavior.
   - `docs/development/build-and-test.md`: CMake presets guide, compiling with strict warnings, running CTest.
   - `docs/development/contributing.md`: TDD workflow (`/tdd`), SOLID principles, Object Calisthenics enforcement.
   - `docs/development/deployment-and-packaging.md`: Kindle OTA packaging with `kindletool`, install/uninstall scripts.

### Phase 5: Architecture Decision Records (ADRs) & Source of Truth (SoT)
1. **ADR Documents**:
   - `docs/adr/README.md`: Index of Architecture Decision Records, status definitions.
   - `docs/adr/0001-modern-cpp20-refactoring.md`: Modern C++20 adoption over legacy C.
   - `docs/adr/0002-zero-exception-error-handling.md`: Zero-exception `Result<T, Error>` on Linux 2.6 ARM.
   - `docs/adr/0003-hardware-abstraction-layer-for-host-testing.md`: Decoupled hardware I/O for 100% host testability.
   - `docs/adr/0004-object-calisthenics-composition.md`: Enforcing ≤2 instance variables via structured composition.
   - `docs/adr/0005-host-mocks-and-native-tests.md`: Native test harness without physical hardware.
   - `docs/adr/0006-backward-compatible-ini-actions.md`: Preserving 2010 INI syntax and action prefixes.
2. **Source of Truth (SoT) Specifications**:
   - `docs/sot/README.md`: SoT directory index and normative status.
   - `docs/sot/coding-standards.md`: Update with modern C++20 conventions and Section 4.2.8 docstrings.
   - `docs/sot/hardware-profile.md`: Authoritative hardware matrix for Kindle 2, DX, and Keyboard 3.
   - `docs/sot/config-schema.md`: Authoritative configuration schema specification.
3. **Master Navigation Update**:
   - `docs/README.md`: Finalize central documentation portal with comprehensive links across all documents.

---

## 4. Verification & Quality Assurance

1. **Docstring Coverage Audit**:
   - Run grep check across `include/launchpad/`:
     ```bash
     grep -rn "/\*\*" include/launchpad/ | wc -l
     ```
   - Verify every header contains file-level and entity-level Doxygen comments.
   - Verify zero compiler warnings or regressions caused by comments:
     ```bash
     cmake --preset host-debug
     cmake --build --preset host-debug
     ```

2. **Automated Host Test Suite**:
   - Run complete test suite:
     ```bash
     ctest --preset host-test
     ```
   - Verify all 36 test cases pass (236 checks).

3. **ARMv6 Cross-Compilation Verification**:
   - Build for Kindle Keyboard release:
     ```bash
     cmake --preset kindle-k3-release
     cmake --build --preset kindle-k3-release
     ```
   - Verify ARMv6 binary builds cleanly with zero warnings.

4. **Documentation Link & Markdown Integrity**:
   - Audit all relative links across `docs/` to ensure no broken references.
   - Verify formatting and readability across all generated guides.
