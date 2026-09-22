# ADR 0003: Hardware Abstraction Layer (HAL) for Host Testing

- **Status**: Accepted
- **Date**: 2026-09-22
- **Deciders**: Core Engineering Team

---

## Context

Kindle Launchpad is intrinsically tied to low-level Linux hardware interfaces:
- **Input Events**: Intercepts physical keypresses via Linux evdev character devices (`/dev/input/event0`, `/dev/input/event1`) using `ioctl(EVIOCGRAB)`.
- **Display Framebuffer**: Writes visual feedback directly to memory-mapped e-ink display buffers (`/dev/fb0`) and triggers e-ink controller screen refreshes via `FBIO_EINK_UPDATE_DISPLAY_AREA` ioctls.
- **Key Injection**: Injects simulated keypresses into the native Kindle Java framework (`cvm`) via character device nodes (`/proc/keypad`, `/proc/fiveway`).
- **Command Execution**: Spawns shell processes via POSIX `fork()`, `exec()`, or `system()`.

In the legacy 2010 implementation, these system calls were hardcoded throughout the code. Consequently, the codebase could not be compiled or executed on developer host machines (x86_64, macOS, or modern Linux desktops) without mock hardware. This prevented automated unit testing, CI pipelines, and rapid test-driven iteration.

---

## Decision

We decided to decouple all low-level hardware interactions behind an abstract **Hardware Abstraction Layer (HAL)** in the `launchpad::hal` namespace:

1. **Abstract Interfaces**:
   - `IInputSource`: Declares `poll_events()`, `grab()`, and `release()`.
   - `IDisplay`: Declares `width()`, `height()`, `fill_rect()`, and `update()`.
   - `ICommandRunner`: Declares `run_command()`.
   - `IKeyInjector`: Declares `inject_key()`.
2. **Production Implementations**:
   - `EvdevInputSource`: Concrete Linux evdev input driver using `ioctl(EVIOCGRAB)` and `poll()`.
   - `KindleDisplay`: Concrete `/dev/fb0` memory-mapper with 4bpp packed pixel packing and e-ink ioctl triggers.
   - `LinuxCommandRunner`: Spawns system shell commands.
   - `LinuxKeyInjector`: Writes scancodes to Kindle `/proc` virtual files.
3. **Mock Test Doubles**:
   - `MockInputSource`: Enqueues synthetic key events programmatically without real device nodes.
   - `MockDisplay`: Emulates a 600×800 or 824×1200 frame buffer in memory, recording draw rects and update calls.
   - `MockCommandRunner`: Records executed commands and returns configured exit codes.
4. **Dependency Injection**:
   - The composition root in `main.cpp` constructs concrete hardware drivers and injects them into `Application` via `ApplicationServices`. Tests instantiate `Application` with mock doubles.

---

## Consequences

### Positive
- **Native Host Testing**: Full end-to-end integration tests execute in < 20 milliseconds on host machines with zero hardware dependencies.
- **Clean Architecture & DIP**: High-level domain logic (`SequenceMatcher`, `ActionRegistry`, `StatusBar`) depends exclusively on abstract contracts, adhering strictly to the Dependency Inversion Principle.
- **Portability**: Adapting Launchpad to different hardware targets (e.g. Kobo, Nook, or newer Kindle Paperwhite models) requires implementing a new HAL driver without touching domain logic.

### Negative / Trade-offs
- **Virtual Function Overhead**: Abstract interfaces use C++ virtual tables (`vtable`). However, because hotkey events occur at human typing speeds (< 20 events per second), virtual dispatch latency (< 3 nanoseconds) is completely negligible.
