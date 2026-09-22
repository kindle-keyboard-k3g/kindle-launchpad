# ADR 0005: Host Mocks and Native Catch2 Tests

- **Status**: Accepted
- **Date**: 2026-09-22
- **Deciders**: Core Engineering Team

---

## Context

Testing software developed for legacy embedded systems like the Amazon Kindle (ARMv6, Linux 2.6.26) traditionally presents severe operational bottlenecks:
1. **Physical Hardware Flashing**: Copying binaries over USB/SSH and manually pressing hotkeys takes minutes per iteration and cannot be run in automated CI pipelines.
2. **Full System Emulation (QEMU)**: Running a complete Kindle OS image in QEMU requires proprietary bootloader dumps, kernel patches, and substantial setup overhead while running slowly.
3. **Hardware Coupling**: Without mockable boundaries, testing edge cases (e.g. sequence timeouts, corrupted INI files, failed ioctls, signal interruptions) is practically impossible on live hardware.

---

## Decision

We decided to decouple all domain logic from the target hardware and execute **100% of unit and integration tests natively on the host development environment** (x86_64 or arm64 Linux/macOS) using **Catch2**:

1. **Native Host Compilation**:
   - The test harness is compiled as a native host executable (`build/host-debug/unit_tests`) via the `host-debug` CMake preset.
2. **In-Memory Mock Test Doubles**:
   - `MockInputSource`: Injects synthetic key events and tracks grab/release state.
   - `MockDisplay`: Emulates a virtual 4bpp display buffer in memory without opening `/dev/fb0`.
   - `MockCommandRunner`: Records dispatched shell commands and returns simulated exit codes without executing live processes.
3. **Catch2 Test Framework**:
   - Utilizes Catch2 for expressive BDD-style assertions (`REQUIRE`, `CHECK`), granular test tagging (`[unit]`, `[integration]`, `[hal]`, `[ui]`), and built-in CTest integration.
4. **Fast Feedback Loop**:
   - The entire suite (36 test cases, 236 assertions) runs in approximately **10 milliseconds** via `ctest --preset host-test`.

---

## Consequences

### Positive
- **Instant TDD Cycle**: Developers can run unit tests continuously during development with sub-second feedback.
- **CI Automation**: Tests execute in standard GitHub Actions / Gitea runners without special emulation or cross-toolchains.
- **Edge-Case Simulation**: Exact timing boundaries (e.g. `HotInterval` timeout expiration down to the exact millisecond) can be verified deterministically.

### Negative / Trade-offs
- **Host vs Target Differences**: Host testing does not detect hardware-specific bugs such as kernel driver deviations, endianness discrepancies, or memory bus alignment faults. Hardware cross-compilation (`kindle-k3-release`) and periodic on-device verification remain necessary.
