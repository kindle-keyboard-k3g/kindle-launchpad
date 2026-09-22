# ADR 0006: Backward-Compatible INI Action Syntax

- **Status**: Accepted
- **Date**: 2026-09-22
- **Deciders**: Core Engineering Team

---

## Context

Kindle Launchpad has supported the Kindle e-ink community since 2010. Over more than a decade, thousands of users across the MobileRead forums created customized configuration files (`launchpad.ini`, `servicecmds.ini`, `fbreader.ini`, etc.) to launch applications, toggle USB networking, take screenshots, and execute scripts.

The legacy syntax defined distinct action execution semantics based on leading prefix characters:
- `!<command>`: Shell execution.
- `@<script>`: Local script execution in `scripts/`.
- `#<keys>`: Emulation of Kindle Java search-bar key sequences.
- Quoted string or raw tokens: Direct key injection.
- Delimiter hotkey sequence syntax: `Shift Shift <key> = <action>`.

When refactoring the codebase into modern C++20, an alternative was considered to adopt a modern configuration format like YAML, TOML, or JSON.

---

## Decision

We decided to maintain **100% strict backward compatibility** with the legacy 2010 INI configuration syntax and action prefixes:

1. **Retain INI Format**: Continue parsing standard `.ini` files with section headers (`[Actions]`, `[Settings]`), key-value pairs, and comments (`#` or `;`).
2. **Preserve Action Prefixes**:
   - `!` $\rightarrow$ `domain::ShellAction`
   - `@` $\rightarrow$ `domain::ScriptAction`
   - `#` $\rightarrow$ `domain::KindleKeysAction`
   - `"..."` $\rightarrow$ `domain::SendKeysAction`
3. **Multi-File Aggregation**:
   - Recursively scan `/mnt/us/launchpad/` for all `*.ini` files and merge action definitions, preserving original priority rules.
4. **Internal Representation via `std::variant`**:
   - Parse legacy strings into a type-safe modern C++20 `std::variant<ShellAction, ScriptAction, KindleKeysAction, SendKeysAction, ScreenshotAction>` without leaking raw string manipulation into the execution layer.

---

## Consequences

### Positive
- **Drop-in Binary Replacement**: Users can replace their legacy `launchpad` binary with the modern C++20 build without modifying any existing `.ini` configuration files.
- **Ecosystem Preservation**: Decade-old community guides, documentation, and keymap packages remain completely valid.
- **Type Safety at the Boundary**: While the external input format remains legacy text, internal handling is strictly type-safe, validated immediately at parse time.

### Negative / Trade-offs
- **Custom Parser Maintenance**: The team must maintain the custom INI parser rather than utilizing a standard third-party JSON/YAML parser (which would have introduced undesirable binary bloat anyway).
