# Command-Line Interface (CLI) Reference

This document provides a comprehensive reference for the Kindle Launchpad executable commands, arguments, and process signals.

---

## 1. Synopsis

```bash
launchpad [OPTIONS]
```

When invoked without options, Kindle Launchpad forks into the background, creates a lockfile at `/var/run/launchpad.pid`, and redirects standard I/O streams to `launchpad.log`.

---

## 2. Command-Line Options

| Option | Argument | Default | Description |
| :--- | :--- | :--- | :--- |
| `-d` | None | Disabled | **Foreground Debug Mode**: Prevents daemonization, runs Launchpad in the active terminal, and prints log output directly to `stderr`. |
| `-kb` | `<device_path>` | `/dev/input/event0` | **Keyboard Device Path**: Overrides the Linux evdev device node used for keyboard event polling. |
| `-fw` | `<device_path>` | `/dev/input/event1` | **Five-Way Device Path**: Overrides the Linux evdev device node used for 5-way joystick controller polling. |
| `-h`, `--help` | None | — | Displays brief usage information and exits. |

### Option Syntax Notes
Device flags accept both spaced and attached syntax:
- Spaced: `launchpad -d -kb /dev/input/event0 -fw /dev/input/event1`
- Attached: `launchpad -d -kb/dev/input/event0 -fw/dev/input/event1`

---

## 3. Exit Status Codes

| Exit Code | Reason | Resolution |
| :--- | :--- | :--- |
| `0` | Clean process termination via `SIGINT` or `SIGTERM`. | Normal shutdown. |
| `1` | **PID lock contention**: Another instance is already running. | Run `killall launchpad` or remove stale `/var/run/launchpad.pid`. |
| `2` | **Invalid arguments**: Unknown command-line flag passed. | Check flags with `launchpad --help`. |
| `3` | **Device initialization failure**: Unable to open input nodes or framebuffer. | Verify permissions and presence of `/dev/input/event*` and `/dev/fb0`. |

---

## 4. POSIX Signal Control

| Signal | Command Example | Effect |
| :--- | :--- | :--- |
| `SIGHUP` | `kill -HUP $(cat /var/run/launchpad.pid)` | **Dynamic Reload**: Re-scans all `*.ini` files and updates hotkey actions without restarting the daemon or releasing input grabs. |
| `SIGTERM` | `kill $(cat /var/run/launchpad.pid)` | **Graceful Termination**: Releases `EVIOCGRAB` input grabs, restores screen content, deletes lockfile, and exits. |
| `SIGINT` | `Ctrl+C` (in foreground mode) | Same as `SIGTERM`. |
