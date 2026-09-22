# Source of Truth: Coding Standards & Engineering Practices

This document defines the core development standards, engineering principles, and practices enforced across **Kindle Launchpad**.

---

## 1. Test-Driven Development (TDD) & `/tdd`

Development of new features, bug fixes, and refactoring must follow the **Test-Driven Development (TDD)** workflow.

### 1.1 The Red-Green-Refactor Cycle
1. **Red**: Write a failing unit or integration test before writing any production code. Run the test via `ctest --preset host-test` to verify that it fails for the expected reason.
2. **Green**: Write the minimal amount of code necessary to make the test pass. Avoid adding speculative or unrequested functionality.
3. **Refactor**: Clean up the implementation to adhere to SOLID, Object Calisthenics, and Clean Code standards while ensuring all tests continue to pass.

### 1.2 Using the `/tdd` Skill
- When starting work on any code change, invoke the `/tdd` skill (or corresponding TDD workflow) to establish test cases up front.
- Isolate pure logic (such as INI parsing, keycode mapping, command tokenization, and hotkey sequence matching) from Linux hardware I/O (`/dev/fb0`, `/dev/input/event*`) behind HAL interfaces to enable fast, host-based native testing.

---

## 2. SOLID Principles in Modern C++20

SOLID principles guide subsystem architecture, header contracts, and class composition:

### 2.1 Single Responsibility Principle (SRP)
- Each class, struct, and function must do exactly one thing and have one reason to change.
- Never mix input event parsing, action execution, and screen drawing in the same class.
- Separate low-level hardware drivers (`EvdevInputSource`, `KindleDisplay`) from configuration parsing (`IniParser`) and state machine management (`SequenceMatcher`).

### 2.2 Open/Closed Principle (OCP)
- Modules must be open for extension but closed for modification.
- Extend functionality (such as new action prefixes or handlers) through lookup tables, polymorphic interfaces (`ICommandRunner`), or configuration files rather than growing monolithic `switch` or `if/else` ladders.

### 2.3 Liskov Substitution Principle (LSP)
- Interchangeable components (such as HAL drivers `EvdevInputSource` vs `MockInputSource`, or `KindleDisplay` vs `MockDisplay`) must adhere to unified contracts and preserve expected behaviors, error codes, and side-effect guarantees.

### 2.4 Interface Segregation Principle (ISP)
- Keep abstract interfaces small, focused, and segregated by domain.
- Clients should not depend on interface methods they do not invoke.
- Provide minimal interfaces (`IInputSource`, `IDisplay`, `ICommandRunner`, `IKeyInjector`) with pure virtual methods.

### 2.5 Dependency Inversion Principle (DIP)
- High-level orchestrators (`Application`, `StatusBar`) must depend on abstractions (`IInputSource`, `IDisplay`), never concrete hardware drivers.
- Inject dependencies via references or pointers at composition root (`main.cpp`) or in test fixtures.

---

## 3. Object Calisthenics Constraints

Object Calisthenics promotes readable, maintainable, and verifiable code through 9 strict structural constraints:

1. **One level of indentation per function**:
   - Extract nested loops, conditionals, and switch statements into named helper functions.
2. **Don't use `else`**:
   - Structure code with guard clauses, early returns, and lookup structures.
3. **Wrap domain primitives**:
   - Avoid raw integers or unbounded strings for domain concepts. Wrap concepts in dedicated value types (`Milliseconds`, `RawKeyCode`, `KeySequence`).
4. **First-class collections**:
   - Any class that manages a collection (`ActionRegistry`, `IniDocument`) should only encapsulate that collection and expose operations through dedicated accessor methods.
5. **One dot/arrow per line**:
   - Avoid deep pointer or method chaining (`ptr->field->subfield`). Assign intermediate variables with meaningful names.
6. **Do not abbreviate**:
   - Use clear, descriptive identifiers. Avoid abbreviations such as `buf`, `len`, `tmp`, `fn`, `pact`. Use `action_definition`, `key_sequence`, `screen_buffer`.
7. **Keep entities small**:
   - Functions: **≤ 15 lines**.
   - Structs/Classes: **≤ 100 lines**.
   - Source modules: **< 200 lines**.
8. **No more than 2 instance variables per entity**:
   - Decompose multi-field classes into cohesive aggregates (`ApplicationServices`, `ApplicationContext`, `MatcherConfig`).
9. **Tell, don't ask (No public getters/setters)**:
   - Expose behavior through methods that execute actions on data rather than leaking internal representations for callers to inspect and modify.

---

## 4. Zero-Exception Error Handling (`core::Result`)

- **Zero Exceptions on Embedded**: Standard C++ exceptions (`throw`, `try`, `catch`) are forbidden in core runtime paths to avoid binary bloat and nondeterministic unwinding latency on ARMv6 targets.
- **Monadic Error Propagation**: All fallible functions must return `core::Result<T, core::Error>`.
- **Compile-Time Checks**: Annotate all functions returning `Result` with `[[nodiscard]]`.
- **No Swallowed Errors**: Explicitly check or propagate errors; never ignore fallible results.

---

## 5. Doxygen Docstring Standards (Section 4.2.8)

Every public header under `include/launchpad/` must contain structured Doxygen documentation:

1. **File Header**:
   ```cpp
   /**
    * @file filename.hpp
    * @brief Concise single-sentence summary of the component.
    * @details In-depth technical description of responsibility, contracts,
    *          and hardware constraints.
    */
   ```
2. **Functions and Methods**:
   - Clear description of purpose.
   - `@param[in,out] param_name` for each parameter explaining constraints, units, and valid ranges.
   - `@return` describing return value and error modes.
   - Exception/Error specification: Explicitly specify `noexcept` or error semantics via `core::Result<T, core::Error>`.
   - `@note` for thread-safety, signal safety, or Kindle hardware quirks (`EVIOCGRAB`, 4bpp packed pixels).
3. **Classes, Structs, and Enums**:
   - Comprehensive `@brief` explaining domain significance.
   - Brief comments explaining each enum variant or public member field.
