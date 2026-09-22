# ADR 0002: Zero-Exception Error Handling via `Result<T, E>`

- **Status**: Accepted
- **Date**: 2026-09-22
- **Deciders**: Core Engineering Team

---

## Context

Kindle e-ink devices operate under strict resource constraints:
- **Processor**: Freescale i.MX31 (399 MHz) or i.MX353 (532 MHz) ARMv6.
- **Memory**: 64 MB (Kindle 2) to 256 MB (Kindle Keyboard 3).
- **Operating System**: Linux 2.6.26 with older Glibc (2.5 / 2.9).

In embedded systems with these constraints, standard C++ exceptions (`throw`, `try`, `catch`) present multiple drawbacks:
1. **Binary Size Bloat**: Exception handling tables (`.eh_frame`) and unwinding runtime code increase executable footprint substantially.
2. **Nondeterministic Latency**: Stack unwinding introduces unpredictable timing, undesirable for low-latency hardware event handling.
3. **Implicit Control Flow**: Hidden control jumps make resource tracking and signal-safety harder to verify.

Conversely, traditional C-style integer return codes (`int 0` on success, negative on error) and `errno` suffer from well-documented flaws:
- Return values can be silently ignored by callers without compiler warnings.
- Functions cannot cleanly return both an error code and a computed value without output pointer parameters (`foo(..., &out)`).

---

## Decision

We decided to implement a **zero-exception architecture** using a type-safe monadic sum-type: `launchpad::core::Result<T, E = Error>`.

1. **Sum-Type Implementation**:
   - Built on `std::variant<T, E>`, allocating no heap memory beyond the larger of `sizeof(T)` or `sizeof(E)`.
   - Explicit specialization `Result<void, E>` for side-effect operations.
2. **Compile-Time Enforcement**:
   - All `Result` returns are annotated with `[[nodiscard]]`, forcing callers to explicitly check or propagate results.
3. **Monadic Combinators**:
   - Provide `map()`, `and_then()`, and `value_or()` for functional transformation and ergonomic chaining without deeply nested conditionals.
4. **Structured Error Type**:
   - `core::Error` encapsulates a machine-readable `ErrorCode` enum and human-readable context message.

---

## Consequences

### Positive
- **Deterministic Embedded Performance**: Zero exception unwinding tables; minimal binary overhead.
- **Fail-Fast Safety**: Unhandled errors cannot silently bypass callers due to `[[nodiscard]]`.
- **Expressive APIs**: Functions cleanly return computed values on success or descriptive errors on failure without pointer out-parameters.

### Negative / Trade-offs
- **Boilerplate Propagation**: Without compiler-level `try` / `?` operators, manual inspection or helper macros are needed when chaining multiple fallible operations.
- **Standard Library Interop**: Standard library methods that throw exceptions (e.g. `std::vector::at`) must either be avoided in favor of bounded indexing or wrapped at system boundaries.
