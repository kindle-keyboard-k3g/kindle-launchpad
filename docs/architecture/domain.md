# Domain Subsystem Architecture

The **Domain subsystem** (`launchpad::domain`) encapsulates all core business rules, hotkey state transitions, keycode translations, and action dispatching. It contains **pure logic** with zero dependencies on hardware drivers or Linux system call nodes.

---

## 1. Hotkey Finite State Machine (`SequenceMatcher`)

The hotkey detection engine operates as a deterministic finite state machine implemented in `SequenceMatcher`.

### 1.1 State Diagram

```
                ┌────────────────────────────────┐
                │             IDLE               │◄─────────────────┐
                │  (Input passes through to OS)  │                  │
                └───────────────┬────────────────┘                  │
                                │                                   │
                                │ Introducer Press + Release        │ Timeout (hot_interval)
                                │ (e.g. "Shift")                    │ OR Cancelled
                                ▼                                   │
                ┌────────────────────────────────┐                  │
                │           CAPTURING            │                  │
                │   (Hardware Grab Active via    │                  │
                │      ioctl(EVIOCGRAB))         │                  │
                └───────┬──────────────┬─────────┘                  │
                        │              │                            │
         Key Event      │              │ Trailer Press              │
   (Append to Sequence) │              │ (e.g. "Enter")             │
                        ▼              ▼                            │
                [Accumulate Key]  [SequenceComplete]                │
                        │              │                            │
                        │              ▼                            │
                        │      [ActionRegistry Lookup]              │
                        │              │                            │
                        └──────────────┴────────────────────────────┘
```

### 1.2 State Transition Rules

#### 1. Idle State
- Keys typed by the user pass through transparently to the Kindle OS framework.
- When the configured `introducer` key (default: `KPKEY_SHIFT`) is pressed, `SequenceMatcher` arms the introducer flag.
- When the introducer key is released with no intermediate keys down, the state transitions to `Capturing` and yields `MatcherActionType::StartCapture`.
- The application orchestrator immediately activates `ScopedInputGrab` (`ioctl(EVIOCGRAB)`), isolating further keypresses from the Kindle UI.

#### 2. Capturing State
- Incoming key presses are translated via `KeyTable::get_substitute_code()` to unify model differences.
- Each key press appends the code to `MatcherContext::sequence` and emits `MatcherActionType::KeyAppended`.
- The application restarts the `hot_interval` countdown timer on each keystroke.
- When the configured `trailer` key (default: `KPKEY_ENTER`) is released, or when a registered terminal sequence matches, the state machine emits `MatcherActionType::SequenceComplete`.

#### 3. Cancellation & Timeout
- If no key is received within `hot_interval` (default: 700 ms), the timer invokes `SequenceMatcher::on_timeout()`.
- The matcher clears its sequence buffer, transitions back to `Idle`, and emits `MatcherActionType::Cancelled`.
- The application releases the hardware input grab and restores the status bar.

---

## 2. Key Definitions & Normalization (`KeyTable`)

Kindle hardware models feature differing keyboard matrices and scancodes:
- **Kindle 2 and Kindle DX**: Share legacy scancode mappings (e.g. `KPKEY_SYM = 94`, `KPKEY_HOME = 98`).
- **Kindle Keyboard 3 (K3)**: Employs updated scancodes (e.g. `K3KEY_SYM = 126`, `K3KEY_HOME = 102`, `K3KEY_FW_PRESS = 194`).

`KeyTable` maintains bidirectional mappings:
1. `find_code(name)`: Case-insensitive symbolic name lookup (e.g. `"Shift"` → `42`, `"Enter"` → `28`, `"Space"` → `57`).
2. `find_name(code)`: Translates scancodes back to canonical names for display.
3. `get_substitute_code(code)`: Normalizes K3-specific scancodes to canonical K2/DX equivalents, ensuring hotkey sequences behave identically across all Kindle models.

---

## 3. Action Modeling & Parsing (`Action`, `ActionParser`)

Actions are modeled as a discriminated union (`std::variant`) wrapped in an `Action` struct:

```cpp
struct Action {
    std::string display_name;
    ActionPayload payload; // std::variant<ShellAction, ScriptAction, KindleKeysAction, SendKeysAction, ScreenshotAction>
};
```

### 3.1 Parsing Grammar
`ActionParser::parse_command(str, display_name, script_dir)` categorizes action strings:

- `!<cmd>` → `ShellAction{command}`: Executes system shell command.
- `@<script>` → `ScriptAction{path}`: Prepends `script_dir` and checks execution permissions.
- `#<query>` → `KindleKeysAction{keys}`: Enters text into the Kindle framework search bar.
- `"<string>"` or tokens → `SendKeysAction{keys}`: Emulates typing via `/proc/keypad`.
- Empty string / Triple-shift → `ScreenshotAction{}`: Triggers framebuffer capture.

---

## 4. Action Registry (`ActionRegistry`)

`ActionRegistry` is a first-class collection (Object Calisthenics Rule 4) encapsulating a `std::map<core::KeySequence, Action>`:

- Default constructor automatically registers the internal screenshot trigger:
  `Shift` + `Shift` + `Shift` → `ScreenshotAction`.
- `register_action(sequence, action)`: Inserts or replaces hotkey bindings.
- `find_action(sequence)`: Performs an exact lookup in $O(\log N)$ time.

---

## 5. Action Execution & Feedback (`ActionExecutor`)

When a sequence completes, `ActionExecutor::execute(const Action& action)` evaluates the variant:

1. **Status Notification**: If an `IStatusNotifier` is attached, displays `action.display_name` on the status bar.
2. **Payload Dispatch**:
   - `ShellAction`: Calls `services_.command_runner.run_command(cmd)`.
   - `ScriptAction`: Spawns script via `command_runner`.
   - `KindleKeysAction`: Uses `services_.key_injector` to activate the Kindle search bar (`Del` key), followed by translated character keystrokes.
   - `SendKeysAction`: Iterates through tokens, resolving them via `KeyTable` or `AsciiTranslator` and writing to `/proc/keypad`.
   - `ScreenshotAction`: Captures framebuffer and writes uncompressed BMP to `/mnt/us/`.
3. **Result Feedback**: Notifies `IStatusNotifier::show_result(success)` to render `"Success!"` or `"Failure."` on the e-ink screen.
