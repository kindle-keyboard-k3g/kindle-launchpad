# Configuration Specification Reference

This document is the authoritative reference for Kindle Launchpad INI configuration files.

---

## 1. File Format & Discovery

Launchpad scans its configuration directory (`/mnt/us/launchpad/`) for all files matching `*.ini`.

- **Encoding**: UTF-8 or standard 7-bit ASCII.
- **Line Endings**: Unix (`\n`) and DOS (`\r\n`) line endings are supported.
- **Comments**: Lines starting with `;` or `#` are comments. Inline comments are not supported.
- **Case Sensitivity**: Section names (`[Settings]`, `[Actions]`) and key names are case-insensitive. Values are case-preserved.

---

## 2. Global `[Settings]` Section

The `[Settings]` section controls daemon timings, event device paths, and network listener configuration.

| Property | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `introducer_key` | String | `Shift` | Symbolic name of the key that arms hotkey matching. |
| `trailer_key` | String | `Enter` | Symbolic name of the key that concludes and executes a sequence. |
| `hot_interval` | Integer (ms) | `700` | Maximum elapsed time allowed between keypresses before sequence resets. |
| `inter_key_delay`| Integer (ms) | `300` | Pause duration between emulated keystrokes when typing strings. |
| `script_directory`| Path | `./scripts` | Directory containing external shell scripts executed via `@`. |
| `keyboard_device` | Path | `/dev/input/event0` | Linux event node for the physical keyboard matrix. |
| `fiveway_device` | Path | `/dev/input/event1` | Linux event node for the 5-way navigation controller. |
| `volume_device` | Path | `/dev/input/event2` | Linux event node for the volume rocker buttons. |
| `rce_enabled` | Boolean | `0` (false) | Enables the UDP multicast Remote Command Entry listener. |
| `rce_port` | Integer | `4444` | Port number for UDP multicast RCE packets. |
| `rce_group` | IP Address | `224.0.0.1` | Multicast group IP address for RCE discovery. |
| `rce_interface` | Interface | `eth0` | Network interface bound for multicast listening. |

### Example `[Settings]` Block
```ini
[Settings]
introducer_key = Shift
trailer_key = Enter
hot_interval = 700
inter_key_delay = 300
script_directory = /mnt/us/launchpad/scripts
keyboard_device = /dev/input/event0
fiveway_device = /dev/input/event1
rce_enabled = 1
rce_port = 4444
```

---

## 3. `[Actions]` Section

The `[Actions]` section defines hotkey chord mappings.

### 3.1 Syntax
```ini
<Sequence> = <Prefix><Payload>
```

- `<Sequence>`: Space-separated list of symbolic key names (e.g. `Shift Shift Space`, `Shift D X`).
- `<Prefix>`: Single character identifying the action execution engine (`!`, `@`, `#`, `"`).
- `<Payload>`: Command line, filename, or string payload.

### 3.2 Action Types & Prefixes

#### 1. Shell Command (`!`)
Spawns an asynchronous shell command via `/bin/sh -c "<command> &"`.
```ini
Shift Shift R = !reboot
Shift Shift T = !date >> /mnt/us/launchpad/time.txt
```

#### 2. Framework Script (`@`)
Executes an executable shell script from `script_directory`.
```ini
Shift Shift W = @toggle_wifi.sh
Shift Shift M = @mount_usb.sh
```

#### 3. Kindle Framework Keys (`#`)
Simulates typing into the Kindle framework search bar. Automatically activates the search bar by injecting the `Del` key, then enters the text.
```ini
Shift Shift K = #wikipedia thermodynamics
Shift Shift B = #store search programming
```

#### 4. Raw Virtual Keystrokes (`"<string>"` or tokens)
Injects keystrokes directly into `/proc/keypad`.
```ini
; Types the literal text string into the active input field
Shift Shift C = "calibre"

; Injects specific symbolic keys
Shift Shift N = Sym FW_DOWN FW_PRESS
```

#### 5. Screenshot Action (Empty or Default)
Captures the current e-ink framebuffer memory to an uncompressed 24bpp BMP file at `/mnt/us/screenshot_<timestamp>.bmp`.
```ini
; Triple Shift is registered automatically by default
Shift Shift Shift = 
```
