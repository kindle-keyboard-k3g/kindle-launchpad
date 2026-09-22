# Contributing Guidelines & Engineering Standards

This document establishes the engineering practices, development rules, and quality gates required for contributing to **Kindle Launchpad**.

---

## 1. Test-Driven Development (TDD)

All new features, refactoring, and bug fixes must follow strict **Test-Driven Development (TDD)**:

```
[RED: Write failing test] ──> [GREEN: Minimal code to pass] ──> [REFACTOR: Clean up]
```

### 1.1 The Golden Rule of TDD
> **No production code without a failing test first.**

1. Write a focused unit test in `tests/unit/` expressing desired behavior or reproducing a defect.
2. Run `ctest --preset host-test` and watch the test **fail**. Verify it fails for the expected reason.
3. Write the minimal amount of code to make the test pass.
4. Refactor the implementation to meet SOLID and Object Calisthenics standards while maintaining 100% green tests.

---

## 2. SOLID Principles in C++20

- **Single Responsibility (SRP)**: Each class and module must have exactly one reason to change. Separate parsing (`IniParser`), storage (`ActionRegistry`), state transitions (`SequenceMatcher`), and execution (`ActionExecutor`).
- **Open/Closed (OCP)**: Extend behavior via composition, configuration, and interface implementation rather than modifying existing control switches.
- **Liskov Substitution (LSP)**: All implementations of `IInputSource`, `IDisplay`, and `ICommandRunner` must fulfill their behavioral contracts identically.
- **Interface Segregation (ISP)**: Interfaces must be minimal and cohesive. Callers must not be forced to depend on unused methods.
- **Dependency Inversion (DIP)**: High-level application orchestrators must depend on abstract interfaces (`IInputSource`, `IDisplay`), never concrete hardware drivers.

---

## 3. Object Calisthenics Constraints

To maximize code clarity, testability, and safety on embedded targets:

1. **One level of indentation per function**: Extract nested logic into named helper methods.
2. **Do not use `else`**: Use early returns, guard clauses, or pattern matching.
3. **Wrap all primitives**: Encapsulate raw values in domain types (`Milliseconds`, `RawKeyCode`).
4. **First-class collections**: Classes wrapping collections (`ActionRegistry`) must expose only that collection's behavior.
5. **One dot per line**: Avoid deep method or pointer chaining (`a->b()->c()`).
6. **No abbreviations**: Use clear, descriptive names (`key_sequence`, `action_executor`).
7. **Keep entities small**:
   - Functions: **≤ 15 lines**
   - Classes / Structs: **≤ 100 lines**
   - Source files: **< 200 lines**
8. **No more than 2 instance variables per entity**: Decompose structs using structured composition aggregates (e.g. `ApplicationServices`, `ApplicationContext`).
9. **Tell, don't ask**: Expose behaviors through methods rather than leaking internal fields via getters and setters.

---

## 4. Documentation & Doxygen Docstring Standards

All public headers under `include/launchpad/` must have complete Doxygen docstrings:

- **File Header**: `@file`, `@brief`, and `@details` explaining the component's role in the architecture.
- **Functions & Methods**:
  - Clear purpose description.
  - `@param[in,out]` for all parameters specifying constraints.
  - `@return` describing return values and error conditions.
  - Explicit specification of `noexcept` or error return via `core::Result<T, Error>`.
  - `@note` for thread-safety, signal-safety, or hardware quirks (`EVIOCGRAB`, 4bpp packing).
- **Types & Enums**: Clear `@brief` explaining domain semantics.
