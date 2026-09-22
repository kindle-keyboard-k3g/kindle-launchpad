# Kindle Launchpad Documentation

Welcome to the internal engineering and architectural documentation for **Kindle Launchpad**.

---

## Documentation Structure

```
docs/
├── README.md               # This document (Documentation entry point)
├── sot/                    # Source of Truth specifications
│   └── coding-standards.md # Engineering rules: TDD, SOLID, Object Calisthenics
└── adr/                    # Architecture Decision Records
```

---

## Core Engineering Rules

All code contributions, refactoring, and feature additions must strictly adhere to the following core engineering standards:

### 1. Test-Driven Development (TDD) & `/tdd`
- **Red-Green-Refactor**: Write a failing unit or integration test before implementing any feature or bug fix.
- **Workflow**: Invoke `/tdd` when starting code changes.
- **Testability**: Separate pure business and parsing logic (such as INI parsing, keycode mapping, command tokenization) from low-level Linux hardware I/O (`/dev/input/*`, `/dev/fb0`) to enable host-based test runs.

### 2. SOLID Principles
- **Single Responsibility (SRP)**: Each function and C module must perform one task with a single reason to change.
- **Open/Closed (OCP)**: Extend functionality via tables, strategy function pointers, and configurations without modifying existing core loops.
- **Liskov Substitution (LSP)**: Input drivers, display handlers, and abstraction layers must preserve contract behaviors.
- **Interface Segregation (ISP)**: Keep header files (`.h`) focused and minimal. Keep private module functions static.
- **Dependency Inversion (DIP)**: Depend on abstractions rather than hardcoded hardware devices. Parameterize device paths and file descriptors.

### 3. Object Calisthenics
1. **One indent level per function**: Extract nested blocks into named helper functions.
2. **No `else`**: Use guard clauses, early returns, or lookup tables.
3. **Wrap domain primitives**: Wrap raw scalars in dedicated typedef structs.
4. **First-class collections**: Encapsulate lists and tables in dedicated structs with focused operations.
5. **One dot/arrow per line**: Avoid deep chaining (`a->b->c`).
6. **No abbreviations**: Use descriptive identifiers (avoid `buf`, `tmp`, `fn`, `pact`).
7. **Keep entities small**: Functions ≤ 15 lines, entities ≤ 100 lines, modules < 200 lines.
8. **≤ 2 instance variables per entity**: Decompose structs into focused sub-structures.
9. **Tell, don't ask**: Expose behaviors through functions rather than leaking struct internals.

---

## Build Presets & Test Execution

Kindle Launchpad uses CMake presets for building and testing:

```bash
# 1. Host debug build and full test suite execution
cmake --preset host-debug
cmake --build --preset host-debug
ctest --preset host-test

# 2. Kindle ARMv6 release cross-compilation
cmake --preset kindle-k3-release
cmake --build --preset kindle-k3-release

# 3. Packaging staging
cmake --build --preset kindle-k3-release --target package-stage
```

---

## Detailed Standards

For the complete specification and guidelines, see:
* **[Coding Standards & Engineering Practices](sot/coding-standards.md)** (`docs/sot/coding-standards.md`)
* **[C++20 Refactoring Architecture Plan](plans/hidden-honking-zephyr.md)** (`docs/plans/hidden-honking-zephyr.md`)

