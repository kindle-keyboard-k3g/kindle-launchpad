# ADR 0001: Modern C++20 Architecture Refactoring

- **Status**: Accepted
- **Date**: 2026-09-22
- **Deciders**: Core Engineering Team

---

## Context

Kindle Launchpad originated in 2010 as a monolithic C99 daemon developed for early Amazon Kindle e-ink devices (Kindle 2, Kindle DX, and Kindle Keyboard 3). While functionally effective, the legacy codebase suffered from significant architectural limitations:
1. **Global Mutable State**: Global variables for configuration settings, screen dimensions, open file descriptors, and sequence buffers made concurrent testing impossible.
2. **Monolithic Procedural Design**: Functions in `launchpad.c` and `inifile.c` exceeded hundreds of lines, interweaving hardware event handling, state machine logic, string parsing, and framebuffer rendering.
3. **No Automated Testing**: The application lacked unit or integration test harnesses; verification required manual flashing and testing on physical Kindle hardware.
4. **Manual Resource Management**: Raw file descriptors and manual memory buffers lacked RAII encapsulation, presenting resource leak risks during error branches.

---

## Decision

We decided to refactor Kindle Launchpad from legacy C into a modern, modular **C++20** architecture:
1. **Adopt C++20 Standards**: Leverage strongly-typed value objects, `std::span`, `std::string_view`, `std::variant`, `std::optional`, and three-way comparison (`operator<=>`).
2. **Layered Modular Subsystems**: Decompose the architecture into cohesive namespaces:
   - `launchpad::core`: Monadic error handling, domain primitives, and RAII primitives.
   - `launchpad::config`: INI parsing and configuration directory aggregation.
   - `launchpad::domain`: Pure hotkey state machine, action registry, and execution.
   - `launchpad::hal`: Hardware abstraction interfaces and device drivers.
   - `launchpad::ui`: Bitmap font rendering, status bar overlays, and BMP screenshot generation.
   - `launchpad::system`: Application orchestration, daemonization, and signal management.
3. **Preserve Target Portability**: Ensure full cross-compilation support for 32-bit ARMv6 (Freescale i.MX31/i.MX35) running Linux 2.6.26 with `arm-linux-gnueabi-g++` and soft-float ABI.

---

## Consequences

### Positive
- **100% Host Test Coverage**: Business logic, sequence matching, and INI parsing can be tested natively on developer machines without physical hardware.
- **Type Safety**: Domain primitives (`Milliseconds`, `RawKeyCode`, `KeySequence`) eliminate parameter transposition bugs.
- **Leak-Free Resource Lifecycle**: RAII wrappers (`ScopedFileDescriptor`, `PidLock`, `ScopedInputGrab`) guarantee deterministic cleanup on all code paths.
- **Zero Runtime Overhead**: C++20 abstractions (templates, `std::string_view`, constexpr tables) compile to lightweight ARM assembly matching or exceeding hand-written C.

### Negative / Trade-offs
- **Toolchain Requirements**: Building for the device requires a modern cross-compiler supporting C++20 (`arm-linux-gnueabi-g++` 11+).
- **Executable Size**: The unstripped C++20 binary contains additional template metadata, though stripping with `-s` and optimization (`-O3`) yields a compact binary (< 200 KB) well within Kindle storage limits.
