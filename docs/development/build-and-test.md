# Building & Testing Guide

This guide details how to build, test, and verify Kindle Launchpad using CMake presets and automated testing frameworks.

---

## 1. CMake Build Presets

Kindle Launchpad defines standardized build presets in `CMakePresets.json`:

| Preset Name | Target Platform | Build Type | Features / Sanitizers |
| :--- | :--- | :--- | :--- |
| `host-debug` | Host (x86_64 / arm64) | Debug (`-O0 -g3`) | Includes full test suite, assertions, debug logging. |
| `host-release` | Host (x86_64 / arm64) | Release (`-O3`) | Optimized, stripped host executable. |
| `kindle-k3-release` | Kindle ARMv6 | Release (`-O3`) | Cross-compiled using `arm-linux-gnueabi-g++`, stripped binary. |

---

## 2. Host Build & Test Workflow

### 2.1 Compiling with Debug Flags & Tests
```bash
# Configure debug preset
cmake --preset host-debug

# Build core library, daemon executable, and test runner
cmake --build --preset host-debug
```

### 2.2 Running Automated Tests with CTest
```bash
# Execute full test suite via CTest preset
ctest --preset host-test --output-on-failure
```

### 2.3 Running Catch2 Runner Directly
For granular test execution or detailed assertion reports, run the test executable directly:

```bash
# Run all tests with standard console output
./build/host-debug/unit_tests

# Filter tests by tag
./build/host-debug/unit_tests "[unit]"         # Run unit tests only
./build/host-debug/unit_tests "[integration]"  # Run end-to-end integration tests
./build/host-debug/unit_tests "[hal]"          # Run HAL mock tests
./build/host-debug/unit_tests "[ui]"           # Run UI and font tests

# Run a specific test case by name
./build/host-debug/unit_tests "SequenceMatcher tracks hotkey state"
```

---

## 3. Strict Compiler Warnings

Kindle Launchpad enforces strict compiler warnings configured in `cmake/CompilerWarnings.cmake`:

```cmake
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Wshadow
-Wnon-virtual-dtor
-Wold-style-cast
-Wcast-align
-Wunused
-Woverloaded-virtual
-Wformat=2
```

All warnings are treated as build errors during CI. Code must compile with **zero warnings**.

---

## 4. Kindle ARMv6 Cross-Compilation

To cross-compile the release binary for Kindle devices:

```bash
# Configure for ARMv6 target
cmake --preset kindle-k3-release

# Compile stripped binary
cmake --build --preset kindle-k3-release
```

The resulting binary is created at `./build/kindle-k3-release/launchpad`.

Verify the target ELF architecture:
```bash
file ./build/kindle-k3-release/launchpad
# Output: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked ... stripped
```
