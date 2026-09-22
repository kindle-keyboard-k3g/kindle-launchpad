# Architecture Decision Records (ADRs)

This directory contains the historical record of architectural decisions made in the **Kindle Launchpad** project.

---

## What is an ADR?

An **Architecture Decision Record (ADR)** captures a significant architectural decision alongside its context, rationale, alternatives considered, and consequences.

Each ADR follows the structure:
- **Title**: Sequential number and concise description (e.g. `0001-modern-cpp20-refactoring.md`).
- **Status**: Current lifecycle state (`Proposed`, `Accepted`, `Superceded`, `Deprecated`).
- **Context**: The problem statement, technical constraints, and motivation.
- **Decision**: The architectural choice made and how it is implemented.
- **Consequences**: Positive effects, trade-offs, limitations, and downstream impacts.

---

## Index of Records

| ADR | Title | Status | Date | Summary |
| :--- | :--- | :--- | :--- | :--- |
| [0001](0001-modern-cpp20-refactoring.md) | Modern C++20 Refactoring | **Accepted** | 2026-09-22 | Refactoring the legacy 2010 C codebase into a modular C++20 architecture. |
| [0002](0002-zero-exception-error-handling.md) | Zero-Exception Error Handling via `Result<T, E>` | **Accepted** | 2026-09-22 | Using monadic sum-types instead of C++ exceptions or raw error codes. |
| [0003](0003-hardware-abstraction-layer-for-host-testing.md) | Hardware Abstraction Layer (HAL) for Host Testing | **Accepted** | 2026-09-22 | Decoupling Linux input/display ioctls behind abstract interfaces. |
| [0004](0004-object-calisthenics-composition.md) | Object Calisthenics Composition for Instance Variables | **Accepted** | 2026-09-22 | Enforcing $\le 2$ instance variables per class via structured service composition. |
| [0005](0005-host-mocks-and-native-tests.md) | Host Mocks and Native Catch2 Tests | **Accepted** | 2026-09-22 | Executing 100% of unit and integration tests natively on host x86_64/arm64. |
| [0006](0006-backward-compatible-ini-actions.md) | Backward-Compatible INI Action Syntax | **Accepted** | 2026-09-22 | Preserving legacy 2010 INI action prefixes (`!`, `@`, `#`, `"keys"`) without changes. |
