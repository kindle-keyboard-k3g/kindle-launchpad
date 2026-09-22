# Getting Started with Kindle Launchpad

This guide provides a step-by-step walkthrough for building, testing, running, and configuring **Kindle Launchpad** for local host development and target Kindle devices.

---

## 1. System Prerequisites

### Host Development Tools
- **Build System**: CMake ≥ 3.22 and Ninja or GNU Make
- **Compiler**: Modern C++20 compliant compiler:
  - GCC ≥ 11.0 (tested with GCC 13 and 15)
  - Clang ≥ 14.0
- **Testing Framework**: Catch2 v3 (fetched automatically via CMake FetchContent or resolved from system libraries)

### Kindle Cross-Compilation (ARMv6 Target)
- **Toolchain**: `arm-linux-gnueabi-g++` and `arm-linux-gnueabi-gcc`
- **Target Architecture**: ARMv6 (Freescale i.MX31 / i.MX35), Linux kernel 2.6.x (glibc 2.5)
- **Packaging Utility**: `kindletool` (optional, required only for building signed OTA `.bin` packages)

---

## 2. Building on Host

Kindle Launchpad provides standardized CMake presets configured in `CMakePresets.json`:

### 2.1 Debug Build & Test Suite
```bash
# Configure debug build
cmake --preset host-debug

# Build core library, executable, and test suite
cmake --build --preset host-debug

# Execute Catch2 unit and integration tests
ctest --preset host-test
```
You can also run the test binary directly for detailed test reporting:
```bash
./build/host-debug/unit_tests -r console
```

### 2.2 Release Build
```bash
# Configure optimized host binary
cmake --preset host-release

# Compile stripped release executable
cmake --build --preset host-release
```
The executable is produced at `./build/host-release/launchpad`.

---

## 3. Running in Foreground (Development Mode)

By default, running `launchpad` without arguments causes it to daemonize (detach into background, fork, redirect standard I/O, and write `/var/run/launchpad.pid`).

During development or debugging, run in **foreground mode** using the `-d` (debug) flag:

```bash
./build/host-debug/launchpad -d
```

### Custom Device Path Overrides
For testing on simulated or alternative event device paths:
```bash
./build/host-debug/launchpad -d -kb /dev/input/event0 -fw /dev/input/event1
```

Available flags:
- `-d`: Run in foreground with verbose console output.
- `-kb <path>`: Path to physical keyboard event device (default: `/dev/input/event0`).
- `-fw <path>`: Path to 5-way joystick controller event device (default: `/dev/input/event1`).

---

## 4. Configuration Basics & First Hotkey

Kindle Launchpad scans its directory (typically `/mnt/us/launchpad/` on device, or current working directory on host) for all `*.ini` files.

### 4.1 Anatomy of `launchpad.ini`

A minimal configuration consists of a `[Settings]` section and an `[Actions]` section:

```ini
[Settings]
; Introducer key that activates hotkey matching
introducer_key = Shift

; Trailer key that terminates and executes the sequence
trailer_key = Enter

; Maximum duration in ms allowed between successive keys
hot_interval = 700

; Key delay for emulated typing in ms
inter_key_delay = 300

; Directory containing executable scripts
script_directory = ./scripts

[Actions]
; Syntax: <Sequence> = <Prefix><Payload>
; Shift + Shift + H runs a shell command
Shift Shift H = !echo "Hello from Launchpad" >> /tmp/launchpad.log

; Shift + Shift + S runs a script from ./scripts/
Shift Shift S = @toggle_wifi.sh

; Shift + Shift + K searches the Kindle framework
Shift Shift K = #wikipedia physics

; Shift + Shift + Shift triggers the internal e-ink screenshot writer
Shift Shift Shift = 
```

### 4.2 Action Types & Prefixes

| Prefix | Description | Example |
| :--- | :--- | :--- |
| `!<cmd>` | **Shell Command**: Spawns `/bin/sh -c "<cmd>"` asynchronously | `Shift Shift R = !reboot` |
| `@<script>` | **Script Action**: Executes a script from `script_directory` | `Shift Shift W = @toggle_wifi.sh` |
| `#<query>` | **Kindle Keys**: Emulates Kindle search bar text entry | `Shift Shift B = #store search books` |
| `"<text>"` | **Raw Keystrokes**: Emulates keyboard keystrokes via `/proc/keypad` | `Shift Shift C = "calibre"` |
| *(None)* | **Internal Action**: Built-in actions like triple-shift screenshot | `Shift Shift Shift = ` |

---

## 5. Cross-Compiling for Kindle Devices

To build the optimized ARMv6 binary for Kindle Keyboard (K3), Kindle DX, or Kindle 2:

```bash
# Configure using ARMv6 cross-toolchain preset
cmake --preset kindle-k3-release

# Compile stripped release binary
cmake --build --preset kindle-k3-release
```

The resulting binary is located at `./build/kindle-k3-release/launchpad`.

Verify the target ELF architecture:
```bash
file ./build/kindle-k3-release/launchpad
# Output: ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), dynamically linked ... stripped
```

---

## 6. Next Steps

- Explore the system architecture in **[Architecture Overview](architecture/overview.md)**.
- Read about the hotkey state machine in **[Domain Architecture](architecture/domain.md)**.
- Reference the full key table in **[Keycodes Reference](reference/keycodes.md)**.
- Review engineering practices in **[Coding Standards](sot/coding-standards.md)**.
