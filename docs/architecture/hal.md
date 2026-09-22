# Hardware Abstraction Layer (HAL) Architecture

The **Hardware Abstraction Layer (HAL)** (`launchpad::hal`) isolates all low-level Linux kernel interfaces, hardware device nodes, and e-ink display ioctls behind clean C++20 abstract interfaces.

---

## 1. Architectural Purpose & Dependency Inversion

In the legacy 2010 C implementation, business logic directly invoked POSIX system calls (`ioctl(fd, EVIOCGRAB)`, `open("/dev/fb0")`, `system()`), rendering host testing impossible.

Under the modern architecture, high-level components (`Application`, `SequenceMatcher`, `ActionExecutor`, `StatusBar`) depend strictly on abstractions:

```
┌────────────────────────────────────────────────────────┐
│                   Domain / UI Layers                   │
│         (SequenceMatcher, ActionExecutor, StatusBar)    │
└───────────────────────────┬────────────────────────────┘
                            │ (Depends on abstractions)
                            ▼
┌────────────────────────────────────────────────────────┐
│                   HAL Interfaces                       │
│    IInputSource      IDisplay       ICommandRunner     │
│                     IKeyInjector                       │
└───────────────┬────────────────────────┬───────────────┘
                │                        │
       Implements                        Implements
                ▼                        ▼
┌──────────────────────────────┐ ┌───────────────────────┐
│     Linux Target Drivers     │ │      Test Doubles     │
│       EvdevInputSource       │ │    MockInputSource    │
│        KindleDisplay         │ │      MockDisplay      │
│      LinuxCommandRunner      │ │   MockCommandRunner   │
│       LinuxKeyInjector       │ └───────────────────────┘
└──────────────────────────────┘
 (Requires Linux kernel 2.6)       (100% Host Native)
```

---

## 2. Input Source & Exclusive Grabbing (`IInputSource`)

### 2.1 The `IInputSource` Interface
```cpp
class IInputSource {
public:
    virtual ~IInputSource() = default;
    virtual core::Result<void> grab() = 0;
    virtual core::Result<void> release() = 0;
    [[nodiscard]] virtual bool is_grabbed() const noexcept = 0;
    virtual std::vector<InputKeyEvent> poll_events(core::Milliseconds timeout) = 0;
};
```

### 2.2 Kernel Grabbing Mechanics (`EVIOCGRAB`)
When `SequenceMatcher` arms after receiving an Introducer key, `EvdevInputSource::grab()` asserts `ioctl(fd, EVIOCGRAB, 1)` on all open event descriptors:
- **Kernel Effect**: The Linux input subsystem stops delivering events to other user-space processes (including Amazon's Java framework `cvm`).
- **Isolation**: Prevents hotkey keystrokes (e.g. typing `Shift Shift Space`) from activating native book reading commands or search dialogues.

### 2.3 RAII Safety via `ScopedInputGrab`
To prevent the device from becoming permanently locked if an unexpected return or error occurs, `ScopedInputGrab` guarantees deterministic release:
```cpp
{
    hal::ScopedInputGrab grab(input_source);
    // Exclusive input grab is guaranteed active here.
    // ...
} // Destructor automatically invokes input_source.release()
```

---

## 3. Display Abstraction (`IDisplay`)

Kindle Launchpad interacts with the e-ink screen through `IDisplay`:

```cpp
class IDisplay {
public:
    virtual ~IDisplay() = default;
    [[nodiscard]] virtual DisplayBounds bounds() const noexcept = 0;
    virtual void clear(uint8_t color = 0xFF) = 0;
    virtual void set_pixel(uint32_t x, uint32_t y, uint8_t gray_level) = 0;
    [[nodiscard]] virtual uint8_t get_pixel(uint32_t x, uint32_t y) const = 0;
    virtual core::Result<void> update(const UpdateRegion& region) = 0;
};
```

- **`bounds()`**: Returns screen geometry (`600x800` for K3/K2, `824x1200` for DX).
- **Pixel Coordinates**: Decouples UI callers from packed 4bpp nibble arithmetic. Callers work in logical $(X, Y)$ coordinates with 8-bit grayscale values ($0$ = black, $255$ = white).
- **`update(region)`**: Flushes modified bounding boxes to physical e-ink hardware using partial refresh waveforms.

---

## 4. Virtual Keystroke Injection (`IKeyInjector`)

```cpp
class IKeyInjector {
public:
    virtual ~IKeyInjector() = default;
    virtual core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) = 0;
    virtual core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) = 0;
};
```

- On physical devices, `LinuxKeyInjector` writes scancodes to `/proc/keypad` and directional commands to `/proc/fiveway`.
- In unit and integration tests, `MockInputSource` captures injected keys into an inspection vector for exact verification.

---

## 5. Host Test Doubles & Deterministic Verification

The HAL test doubles (`MockInputSource`, `MockDisplay`, `MockCommandRunner`) enable comprehensive testing on standard x86_64 host machines:

```cpp
TEST_CASE("Application handles hotkey chord and executes shell action", "[integration]") {
    hal::MockInputSource input_mock;
    hal::MockDisplay display_mock({600, 800, 4});
    hal::MockCommandRunner command_mock;

    // Queue simulated hardware key events: Shift (press/release), Space (press/release)
    input_mock.queue_event({key_codes::KPKEY_SHIFT, domain::KeyEventType::Pressed});
    input_mock.queue_event({key_codes::KPKEY_SHIFT, domain::KeyEventType::Released});
    input_mock.queue_event({key_codes::KPKEY_SPACE, domain::KeyEventType::Pressed});
    input_mock.queue_event({key_codes::KPKEY_SPACE, domain::KeyEventType::Released});

    // Run test cycle...
    REQUIRE(command_mock.executed_commands().size() == 1);
    REQUIRE(command_mock.executed_commands()[0] == "/bin/custom_action.sh");
}
```
Zero hardware dependencies are needed during CI or local development.
