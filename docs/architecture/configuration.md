# Configuration Subsystem Architecture

The **Configuration subsystem** (`launchpad::config`) handles declarative INI file parsing, settings aggregation across multiple files, and dynamic runtime reloading without daemon restarts.

---

## 1. INI Parser Architecture (`ini_parser.hpp`)

Kindle Launchpad uses a standalone, pure-logic parser designed for robust handling of standard `.ini` files found on embedded systems.

### 1.1 Grammar & Parsing Rules
- **Sections**: Enclosed in square brackets, e.g. `[Settings]`, `[Actions]`. Whitespace around section names is ignored.
- **Key-Value Pairs**: Format `Key = Value`. Keys and values are trimmed of leading and trailing whitespace.
- **Comments**: Lines beginning with `;` or `#` are treated as comments and discarded. Empty lines are ignored.
- **Order Preservation**: Section and entry orders are preserved exactly as defined in the source file.

### 1.2 `IniDocument` Model
The parser constructs an `IniDocument`, providing typesafe query access:

```cpp
class IniDocument {
public:
    void add_entry(const std::string& section, std::string key, std::string value);
    [[nodiscard]] bool has_section(std::string_view section) const noexcept;
    [[nodiscard]] std::optional<std::string> get_string(std::string_view section, std::string_view key) const;
    [[nodiscard]] int get_int(std::string_view section, std::string_view key, int default_value = 0) const;
    [[nodiscard]] const std::vector<IniEntry>& get_section_entries(std::string_view section) const;
    [[nodiscard]] std::vector<std::string> get_section_names() const;
};
```

---

## 2. Multi-File Configuration Scanner (`config_scanner.hpp`)

Rather than relying on a single monolithic configuration file, Kindle Launchpad allows modular `.ini` files (e.g. `servicecmds.ini`, `fbreader.ini`, `terminal.ini`).

`ConfigScanner::scan_directory(const std::string& dir)` aggregates all configurations using deterministic precedence rules:

```
                  ┌──────────────────────┐
                  │ Scan /mnt/us/launchpad│
                  └──────────┬───────────┘
                             │
            ┌────────────────┴────────────────┐
            ▼                                 ▼
   [launchpad.ini]                   [*.ini (alphabetical)]
   - Sets baseline [Settings]        - servicecmds.ini
   - Appends [Actions]               - fbreader.ini
            │                        - terminal.ini
            │                        - Overrides [Settings] if present
            │                        - Appends new [Actions]
            └────────────────┬────────────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │  ScannedConfig  │
                    │ - Settings      │
                    │ - All Actions   │
                    └─────────────────┘
```

### 2.1 Precedence & Merging Strategy
1. **Primary Config (`launchpad.ini`)**: If `launchpad.ini` exists in the scanned directory, it is loaded first to establish the baseline `LaunchpadSettings` (introducer key, timeout intervals, device paths).
2. **Modular Extension Files**: All remaining files matching `*.ini` are scanned in alphabetical order. Any `[Settings]` defined in later files override previous values.
3. **Action Aggregation**: All `[Actions]` entries from all scanned files are collected sequentially into `ScannedConfig::actions`. Later actions with duplicate key sequences override earlier entries.

---

## 3. Dynamic Configuration Reloading (`SIGHUP`)

Kindle Launchpad supports live configuration reloading without restarting the process or losing input grabs:

```
[User / Script] ──> kill -HUP <pid>
                         │
                         ▼
             [SignalHandler: flag atomic]
                         │
                         ▼ (Main loop tick)
             [Application::check_reload()]
                         │
                         ├─> ConfigScanner::scan_directory()
                         ├─> ActionRegistry::clear() & rebuild
                         ├─> SequenceMatcher::update_settings()
                         └─> Logger::info("Configuration reloaded")
```

1. An external process or user sends `SIGHUP` (e.g. via `Shift` `Shift` `L` reload hotkey).
2. `SignalHandler::handle_signal` sets an async-signal-safe atomic flag (`reload_requested_`).
3. During the next iteration of the main polling loop, `Application::process_events` detects the flag.
4. `ConfigScanner::scan_directory` re-reads all `.ini` files from disk.
5. The `ActionRegistry` is repopulated with updated actions, and timeouts in `SequenceMatcher` are refreshed.
6. The status bar displays a brief notification confirming successful reload.
