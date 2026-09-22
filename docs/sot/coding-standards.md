# Source of Truth: Coding Standards & Engineering Practices

This document defines the core development standards, engineering principles, and practices enforced across this repository.

---

## 1. Test-Driven Development (TDD) & `/tdd`

Development of new features, bug fixes, and refactoring must follow the **Test-Driven Development (TDD)** workflow.

### 1.1 The Red-Green-Refactor Cycle
1. **Red**: Write a failing unit or integration test before writing any production code. Run the test to verify that it fails for the expected reason.
2. **Green**: Write the minimal amount of code necessary to make the test pass. Avoid adding unrequested functionality.
3. **Refactor**: Clean up the implementation to adhere to SOLID, Object Calisthenics, and Clean Code standards while ensuring all tests continue to pass.

### 1.2 Using the `/tdd` Skill
- When starting work on any code change, invoke the `/tdd` skill (or corresponding TDD workflow) to establish test cases up front.
- Isolate pure logic (such as INI parsing, keycode mapping, command string tokenization, and hotkey sequence matching) from Linux hardware I/O (`/dev/fb0`, `/dev/input/event*`) to enable fast, host-based native testing.

---

## 2. SOLID Principles in Systems C

While C is a procedural language, SOLID principles apply directly to module architecture, header contracts, and function composition.

### 2.1 Single Responsibility Principle (SRP)
- Each function and module must do exactly one thing and have one reason to change.
- Never mix input event parsing, action execution, and screen drawing in the same function or file.
- Separate low-level hardware ioctls (`screen.c`, `screenshot.c`) from configuration parsing (`inifile.c`) and state machine management (`launchpad.c`).

### 2.2 Open/Closed Principle (OCP)
- Modules must be open for extension but closed for modification.
- Extend functionality (such as new action prefixes or handlers) through lookup tables, strategy function pointers, or configuration files rather than growing monolithic `switch` or `if/else` ladders.

### 2.3 Liskov Substitution Principle (LSP)
- Interchangeable components (such as device input drivers or display handlers) must adhere to unified contracts and preserve expected behaviors, error codes, and side-effect guarantees.

### 2.4 Interface Segregation Principle (ISP)
- Keep header files (`.h`) small, focused, and segregated by domain.
- Clients should not depend on header declarations or types they do not use.
- Keep module-internal helper declarations `static` within the `.c` file and out of public headers.

### 2.5 Dependency Inversion Principle (DIP)
- High-level orchestrators should depend on abstractions (e.g. abstract event streams, generic buffer writers, function pointers) rather than hardcoded hardware devices.
- Parameterize device paths and file descriptors to facilitate mock injection during testing.

---

## 3. Object Calisthenics (Adapted for Systems C)

Object Calisthenics promotes readable, maintainable, and verifiable code through 9 strict structural constraints:

1. **One level of indentation per function**:
   - Extract nested loops, conditionals, and switch statements into named helper functions.
2. **Don't use `else`**:
   - Structure code with guard clauses, early returns, and lookup structures.
3. **Wrap domain primitives**:
   - Avoid raw integers or unbounded strings for domain concepts. Wrap concepts (keycodes, key sequences, action types, timers) in dedicated `typedef struct` or enumerated types.
4. **First-class collections**:
   - Any struct that manages an array, list, or table of items (e.g. key tables, action lists) should only encapsulate that collection and expose operations through dedicated accessor functions.
5. **One dot/arrow per line**:
   - Avoid deep pointer or struct member chaining (`ptr->field->subfield`). Assign intermediate variables with meaningful names.
6. **Do not abbreviate**:
   - Use clear, descriptive identifiers. Avoid abbreviations such as `buf`, `len`, `tmp`, `fn`, `pact`. Use `action_definition`, `key_sequence`, `screen_buffer`.
7. **Keep entities small**:
   - Functions: **≤ 15 lines**.
   - Structs/Entities: **≤ 100 lines**.
   - Source modules: **< 200 lines**. Split navigation, parsing, and execution into distinct modules.
8. **No more than 2 instance variables per entity**:
   - Decompose multi-field structs into small, cohesive component structs.
9. **Tell, don't ask (No public getters/setters)**:
   - Expose behavior through functions that execute actions on data rather than leaking internal representations for callers to inspect and modify.

---

## 4. Fail-Fast & Defensive Systems Programming

- **Validate at boundaries**: Check inputs, configuration values, buffer bounds, and file paths immediately upon entering a function.
- **Check all system call returns**: Always check return values from `read()`, `write()`, `ioctl()`, `malloc()`, and `system()`.
- **Never swallow errors**: Log context and error reasons before returning failure codes or handling recovery.
- **Resource cleanup**: Always release file descriptors, lockfiles, grabbed devices, and allocated memory on both normal exit and error exit paths.
