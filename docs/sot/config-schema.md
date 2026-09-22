# Source of Truth: Configuration File Schema

This document defines the normative **Source of Truth** for the syntax, sections, parameters, and action formats recognized by **Kindle Launchpad** across all `.ini` configuration files.

---

## 1. Syntax & Structural Rules

1. **File Format**: Standard INI file structure containing section headers, key-value pairs, and comments.
2. **File Encoding**: Standard ASCII or UTF-8 without Byte Order Mark (BOM).
3. **Section Headers**: Declared inside square brackets on a line by themselves (e.g. `[Settings]`, `[Actions]`).
4. **Key-Value Pairs**: Formatted as `<KeySequence> = <ActionString>`. Whitespace before and after the `=` delimiter is automatically trimmed.
5. **Comments**: Lines beginning with `#` or `;` are ignored as comments. Trailing inline comments are not supported.
6. **Case Sensitivity**: Section names (`[Settings]`, `[Actions]`) and key names (`HotInterval`, `Introducer`) are case-insensitive. Hotkey sequence tokens (e.g. `Shift`, `Enter`, `A`) are case-insensitive. Action payload strings (shell commands, script names) preserve exact casing.

---

## 2. File Precedence & Aggregation

1. **Directory Aggregation**: Launchpad scans `/mnt/us/launchpad/` for all `*.ini` files.
2. **Base Configuration**: `launchpad.ini` is parsed first to initialize default `[Settings]` values.
3. **Auxiliary Files**: Additional `.ini` files (`servicecmds.ini`, `sample.ini`, etc.) are parsed in alphabetical order.
4. **Action Collisions**: If multiple `.ini` files bind the identical hotkey sequence, the last parsed file takes precedence and overwrites previous definitions.

---

## 3. `[Settings]` Section Schema

The `[Settings]` section configures global daemon behavior, timing thresholds, and remote trigger services:

| Field Name | Type | Default Value | Valid Range | Description |
| :--- | :--- | :--- | :--- | :--- |
| `HotInterval` | Integer | `700` | $100$ – $5000$ ms | Inactivity timeout. If this interval elapses between keypresses during a sequence, the sequence is cancelled and hardware is released. |
| `Introducer` | String | `Shift` | Valid Key Name | The hardware key required to initiate a hotkey sequence. Must be pressed twice within `HotInterval`. |
| `Trailer` | String | `Enter` | Valid Key Name | Optional sequence completion key. If specified, actions only execute after pressing the trailer key. |
| `FeedbackSecs`| Integer | `2` | $1$ – $10$ s | Duration to display the status bar overlay ("^[...]", "Success!", "Failure.") on the e-ink screen. |
| `EnableRCE` | Boolean | `0` | `0` or `1` | Enables the Remote Command Entry UDP multicast server when set to `1`. |
| `RCEPort` | Integer | `4444` | $1024$ – $65535$ | UDP port number for the RCE multicast listener. |
| `RCEMulticastIP`| String | `224.0.0.1` | Valid IPv4 | Multicast group IP address for incoming RCE trigger packets. |
| `RCEEnableString`| String| `""` | Any ASCII | Secret authorization passphrase required in RCE packets before executing commands. |

---

## 4. `[Actions]` Section Schema

The `[Actions]` section maps key sequences to execution handlers.

### 4.1 Sequence Key Syntax
- Sequences consist of space-separated symbolic key names:
  ```ini
  [Actions]
  <Key1> <Key2> = <ActionPayload>
  ```
- Because `Introducer` (`Shift`) must precede every sequence, only the subsequent keys are listed:
  ```ini
  # Triggered by: Shift Shift T
  T = !/mnt/us/usbnet/bin/usbnet toggle

  # Triggered by: Shift Shift O B
  O B = @open_book.sh
  ```

### 4.2 Action Payload Prefixes

The leading character of the action string determines the execution engine:

| Prefix | Engine | Target Payload | Example |
| :--- | :--- | :--- | :--- |
| `!` | **Shell Execution** | System command executed via `/bin/sh -c` | `!killall -9 cvm` |
| `@` | **Script Execution** | Shell script located under `scripts/` directory | `@toggle_wifi.sh` |
| `#` | **Framework Keystrokes** | Injects keystroke search macro (`Del` + string + `Enter`) | `#help` |
| None / `"` | **Key Injection** | Virtual scancodes injected directly into `/proc/keypad` | `"text to type"` |

---

## 5. Built-in Reserved Sequences

| Key Sequence | Fixed Action | Overridable |
| :--- | :--- | :--- |
| `Shift Shift Shift` (Triple-Shift) | Captures framebuffer and saves uncompressed BMP screenshot to `/mnt/us/screenshot_<timestamp>.bmp` | No |
| `Shift Shift [Timeout]` | Cancels pending hotkey sequence and releases input grabs | No |
