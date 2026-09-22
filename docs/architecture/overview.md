# Architecture Overview

**Kindle Launchpad** is a background daemon and hotkey launcher designed for Amazon Kindle e-ink devices with physical hardware keyboards (Kindle Keyboard / K3, Kindle DX, and Kindle 2).

This document outlines the high-level system architecture, layered decomposition, dependency injection hierarchy, and data flow of the modern C++20 codebase.

---

## 1. Architectural Philosophy & Constraints

Kindle Launchpad is engineered specifically for resource-constrained embedded Linux systems running on Freescale i.MX31/i.MX35 ARMv6 processors with 32MB–256MB of RAM and Linux 2.6.x kernels.

The architecture adheres to four foundational engineering tenets:

1. **Strict Dependency Inversion (DIP)**: Pure business logic (hotkey sequence matching, INI configuration parsing, command dispatching) has **zero coupling** to low-level Linux hardware devices (`/dev/input/*`, `/dev/fb0`). All hardware interactions are mediated through abstract C++20 interfaces.
2. **Zero-Exception Runtime**: Because C++ exceptions increase binary size and overhead on embedded glibc 2.5 systems, Launchpad uses a monadic `core::Result<T, Error>` sum-type.
3. **Object Calisthenics**: Maximum 1 indentation level per function, no `else` keywords, wrapped domain primitives (`Milliseconds`, `RawKeyCode`), first-class collections (`ActionRegistry`), small cohesive classes (≤100 lines), and maximum 2 instance variables per entity via composition.
4. **100% Host Testability**: The entire daemon lifecycle, key sequence matching, and status bar rendering can be simulated and verified natively on the host workstation without physical hardware.

---

## 2. Layered Architecture Decomposition

The codebase is organized into six strictly segregated subsystems:

```
┌─────────────────────────────────────────────────────────────┐
│                       system/                               │
│     Application, Daemon, SignalHandler, PidLock, RceServer  │
└──────────────┬───────────────────────────────┬──────────────┘
               │                               │
               ▼                               ▼
┌─────────────────────────────┐ ┌─────────────────────────────┐
│            ui/              │ │           domain/           │
│  StatusBar, FontRenderer,   │ │  SequenceMatcher, Registry, │
│     ScreenshotWriter        │ │    ActionExecutor, KeyTable │
└──────────────┬──────────────┘ └──────────────┬──────────────┘
               │                               │
               ▼                               ▼
┌─────────────────────────────────────────────────────────────┐
│                           hal/                              │
│   IInputSource, IDisplay, ICommandRunner, IKeyInjector      │
│   (EvdevInputSource, KindleDisplay, LinuxCommandRunner...)  │
└──────────────────────────────┬──────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────┐ ┌─────────────────────────────┐
│          config/            │ │           core/             │
│   IniParser, IniDocument,   │ │  Result<T,E>, Units, RAII,  │
│   ConfigScanner, Settings   │ │  ScopedFileDescriptor, Log  │
└─────────────────────────────┘ └─────────────────────────────┘
```

### 2.1 Core Subsystem (`launchpad::core`)
Foundational utilities with zero internal dependencies:
- **`Result<T, E>`**: Monadic expected value container for fail-fast error propagation.
- **`Milliseconds` / `RawKeyCode` / `KeySequence`**: Strongly typed domain primitives preventing unit and type confusion.
- **`ScopedFileDescriptor`**: Move-only RAII guard guaranteeing deterministic POSIX file descriptor closure.
- **`Logger`**: Centralized thread-safe diagnostic logger with automatic console/file routing.

### 2.2 Configuration Subsystem (`launchpad::config`)
Pure declarative configuration parsing:
- **`IniParser`**: Tokenizes and validates standard INI syntax, handling comments, whitespace, and section grouping.
- **`IniDocument`**: In-memory queryable representation of parsed sections and entries.
- **`LaunchpadSettings`**: Strongly-typed model of daemon behavior, timeouts, and device paths.
- **`ConfigScanner`**: Multi-file aggregator that scans `/mnt/us/launchpad/*.ini`, applying `launchpad.ini` defaults while appending modular actions.

### 2.3 Domain Subsystem (`launchpad::domain`)
Pure business logic and hotkey state transitions:
- **`KeyTable`**: Static mapping between Linux evdev keycodes, symbolic names (`Shift`, `Enter`), and Kindle device codes.
- **`AsciiTranslator`**: Translates ASCII characters into Kindle framework search-bar keystroke scripts.
- **`Action`**: Modern `std::variant` modeling `ShellAction`, `ScriptAction`, `KindleKeysAction`, `SendKeysAction`, and `ScreenshotAction`.
- **`ActionRegistry`**: First-class collection mapping `KeySequence` chords to executable `Action`s.
- **`SequenceMatcher`**: Finite state machine tracking key presses, timeout intervals (`hot_interval`), and sequence terminations.
- **`ActionExecutor`**: Dispatches matched actions to injected HAL runners.

### 2.4 Hardware Abstraction Layer (`launchpad::hal`)
Interface definitions and concrete hardware drivers:
- **`IInputSource`**: Polling interface and `ScopedInputGrab` RAII guard wrapping `ioctl(EVIOCGRAB)`. Concrete: `EvdevInputSource`.
- **`IDisplay`**: Framebuffer dimensions and e-ink update trigger contracts. Concrete: `KindleDisplay`.
- **`ICommandRunner`**: Shell execution interface. Concrete: `LinuxCommandRunner`.
- **`IKeyInjector`**: Virtual keyboard simulator writing to `/proc/keypad`. Concrete: `LinuxKeyInjector`.
- **Test Doubles**: `MockInputSource`, `MockDisplay`, and `MockCommandRunner` for automated host tests.

### 2.5 UI Subsystem (`launchpad::ui`)
Visual feedback on the e-ink display:
- **`FontRenderer`**: Renders embedded 12x22 1-bit bitmap glyphs into 4bpp packed pixel buffers.
- **`StatusBar`**: Renders transient bottom-right feedback overlays (`^[...]`, `Success!`, `Failure.`) with pixel save/restore.
- **`ScreenshotWriter`**: Extracts framebuffer memory and serializes uncompressed 24bpp BMP files.

### 2.6 System Subsystem (`launchpad::system`)
Daemon orchestration and platform integration:
- **`CliOptions`**: Command-line flag parsing (`-d`, `-kb`, `-fw`).
- **`SignalHandler`**: Async-signal-safe POSIX signal handling (`SIGINT`, `SIGTERM`, `SIGHUP`).
- **`PidLock`**: Non-blocking `flock(LOCK_EX | LOCK_NB)` file lock preventing duplicate daemon instances.
- **`Daemon`**: Linux daemonization (`fork`, `setsid`, stdio redirection).
- **`RceServer`**: Optional background UDP multicast listener for remote triggers.
- **`Application`**: Composition root running the main polling event loop.

---

## 3. Composition Root & Dependency Injection

The entire dependency tree is composed at application startup in `src/main.cpp`:

```cpp
// 1. Hardware drivers instantiated at application root
auto input_source = std::make_unique<hal::EvdevInputSource>(...);
auto display = std::make_unique<hal::KindleDisplay>();
auto command_runner = std::make_unique<hal::LinuxCommandRunner>();
auto key_injector = std::make_unique<hal::LinuxKeyInjector>();

// 2. Services bundled via ApplicationServices aggregate
system::ApplicationServices services{
    .input = std::move(input_source),
    .display = std::move(display),
    .command_runner = std::move(command_runner),
    .key_injector = std::move(key_injector)
};

// 3. Injected into Application orchestrator
system::Application app(options, std::move(services));
return app.run();
```

---

## 4. End-to-End Hotkey Execution Flow

When a user types a hotkey chord (e.g. `Shift` `Shift` `Space`), the event traverses the system through a deterministic pipeline:

```
[Physical Keypad]
       │
       ▼ (Linux input subsystem)
[/dev/input/event0]
       │
       ▼
[EvdevInputSource::poll_events()]
       │
       ▼ (InputKeyEvent: code, pressed)
[SequenceMatcher::process_key()] ──────────────┐
       │ (State: Introducer -> Matching)       │ (Hot interval timeout)
       │                                       ▼
       │                              [Reset to Idle]
       ▼ (Trailer key or terminal chord)
[SequenceMatcher::MatcherResult::Matched]
       │
       ▼
[ActionRegistry::find(sequence)]
       │
       ▼ (Action variant)
[ActionExecutor::execute(action)]
       ├───────────────────┬───────────────────┐
       ▼                   ▼                   ▼
[LinuxCommandRunner] [LinuxKeyInjector] [ScreenshotWriter]
       │                   │                   │
       ▼                   ▼                   ▼
  (/bin/sh)         (/proc/keypad)       (/mnt/us/*.bmp)
       │
       ▼
[StatusBar::show_feedback("Success!")]
       │
       ▼
[KindleDisplay::update_area()] ──> (E-Ink Flash)
```

1. **Hardware Grab**: Upon recognizing the Introducer key (`Shift`), `SequenceMatcher` transitions to the active matching state. The input source activates `ScopedInputGrab` (`ioctl(EVIOCGRAB)`), isolating input from the Kindle native framework.
2. **Buffering & Timing**: Keys are accumulated into a `KeySequence`. If the duration between keys exceeds `hot_interval`, the matcher aborts, releases input grab, and restores the display.
3. **Execution**: When the Trailer key (`Enter`) or a recognized terminal chord is pressed, the matcher yields a `Matched` result. `ActionExecutor` resolves the payload and invokes the appropriate HAL driver.
4. **Status Feedback**: `StatusBar` blits visual text feedback to the bottom-right corner of the e-ink display and triggers an e-ink partial refresh ioctl.
