# Source of Truth (SoT) Specifications

The documents in this directory represent the **normative Source of Truth** for architectural, engineering, hardware, and configuration specifications across **Kindle Launchpad**.

---

## Normative Hierarchy

When conflicts or ambiguities arise during implementation, code review, or documentation updates, specifications in `docs/sot/` take precedence over secondary guides:

1. **System & Platform Directives**: Global engineering rules and constraints.
2. **Source of Truth (`docs/sot/`)**: Authoritative technical contracts defined herein.
3. **Architecture Decision Records (`docs/adr/`)**: Historical rationale for architectural choices.
4. **Subsystem Guides & References (`docs/architecture/`, `docs/reference/`)**: Descriptive explanations and usage manuals.

---

## Directory Index

| Specification | Document | Summary |
| :--- | :--- | :--- |
| **Engineering Standards** | [coding-standards.md](coding-standards.md) | Modern C++20 standards, TDD, SOLID, Object Calisthenics, and Doxygen docstring rules. |
| **Hardware Profiles** | [hardware-profile.md](hardware-profile.md) | Authoritative hardware capabilities, screen resolutions, and Linux evdev device node mappings for K2, DX, and K3. |
| **Configuration Schema** | [config-schema.md](config-schema.md) | Authoritative syntax, fields, types, and defaults for `.ini` configuration files (`[Settings]` and `[Actions]`). |
