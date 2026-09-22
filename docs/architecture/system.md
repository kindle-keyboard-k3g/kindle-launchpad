# System Subsystem Architecture

The **System subsystem** (`launchpad::system`) manages application lifecycle, daemonization, POSIX signals, lockfiles, command-line arguments, and the main event loop.

---

## 1. Daemonization & Lifecycle (`Daemon`)

By default, Kindle Launchpad operates as a detached Unix background daemon:

```
[Main Entry Point (main.cpp)]
               │
               ▼
       [CliOptions::parse()]
               │
    Is foreground (-d)?
       ├── Yes ──> Continue in current terminal session
       │
       └── No ───> [Daemon::daemonize()]
                         │
                         ├─> 1. fork() + parent exit
                         ├─> 2. setsid() (creates new session)
                         ├─> 3. chdir("/") (prevents unmount locking)
                         ├─> 4. umask(022)
                         └─> 5. redirect stdio to /dev/null or log
```

---

## 2. PID File Locking (`PidLock`)

To prevent multiple instances from running concurrently and competing for `EVIOCGRAB` input devices, `PidLock` enforces a single active instance:

- **Lockfile Path**: `/var/run/launchpad.pid` (or `./launchpad.pid` in user mode).
- **Mechanism**: Non-blocking `flock(fd, LOCK_EX | LOCK_NB)`.
- **Fail-Fast**: If another instance holds the lock, Launchpad logs an error and exits immediately with status 1.
- **Cleanup**: The `PidLock` destructor unlinks the lockfile and closes the descriptor on process termination.

---

## 3. POSIX Signal Dispatching (`SignalHandler`)

`SignalHandler` configures signal handlers using `sigaction()`:

| Signal | Handler Action | Application Behavior |
| :--- | :--- | :--- |
| `SIGINT` | Sets `shutdown_requested_ = true` | Exits main polling loop, ungrabs hardware, releases PID lock. |
| `SIGTERM` | Sets `shutdown_requested_ = true` | Graceful termination identical to SIGINT. |
| `SIGHUP` | Sets `reload_requested_ = true` | Re-scans `*.ini` files and refreshes action bindings without restart. |

All handlers set atomic flags (`std::atomic<bool>`), guaranteeing strict async-signal-safety.

---

## 4. Main Event Loop Orchestration (`Application`)

The central event loop in `Application::run()` coordinates polling, timeouts, and action dispatching:

```
                     ┌──────────────────────────────┐
                     │     Application::run()       │
                     └──────────────┬───────────────┘
                                    │
    ┌───────────────────────────────┴───────────────────────────────┐
    ▼                                                               │
┌───────────────────────┐                                           │
│ Check Shutdown Flag   │ ──(Shutdown requested)──> [Exit Event Loop]
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│ Check Reload (SIGHUP) │ ──(Reload requested)────> [reload_configuration()]
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│ Check Inactive Timer  │ ──(hot_interval elapsed)─> [SequenceMatcher::on_timeout()]
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│ Poll Input Events     │ ──> Read events from IInputSource
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│ Process Key Event     │ ──> SequenceMatcher::process_key_event()
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│ Sequence Completed?   │ ──(Yes)─────────────────> [ActionRegistry::find()]
└───────────────────────┘                                    │
                                                             ▼
                                                    [ActionExecutor::execute()]
```

---

## 5. Remote Command Entry (`RceServer`)

`RceServer` provides an optional network-triggered command execution interface:
- **Transport**: UDP multicast on `239.1.2.3:10000` (or configured IP/port).
- **Security**: Requires matching `enable_string` passphrase before commands are executed.
- **Worker Thread**: Runs on a background thread (`std::thread`), leaving the main input polling loop unblocked.
