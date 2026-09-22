# Core Subsystem Architecture

The **Core subsystem** (`launchpad::core`) provides fundamental abstractions, primitives, error models, and system utilities utilized across all other layers of Kindle Launchpad. It has **zero dependencies** on other subsystems.

---

## 1. Zero-Exception Error Handling (`Result<T, E>`)

### 1.1 Motivation & Embedded Constraints
Kindle devices run Linux 2.6 on ARMv6 with glibc 2.5. In such environments:
- Enabling C++ exceptions (`-fexceptions`) introduces significant code size overhead (unwind tables, `.eh_frame` sections).
- Exception throwing incurs nondeterministic latency spikes that interfere with real-time hardware key polling loops.
- Launchpad compiles with strict modern flags, using an explicit, monadic sum-type for all operations that may fail.

### 1.2 The `Result<T, E>` Sum-Type
Defined in `include/launchpad/core/result.hpp`:

```cpp
template <typename T, typename E = Error>
class Result {
public:
    [[nodiscard]] bool is_ok() const noexcept;
    [[nodiscard]] bool is_err() const noexcept;
    explicit operator bool() const noexcept;

    [[nodiscard]] const T& value() const;
    [[nodiscard]] const E& error() const;
    [[nodiscard]] T value_or(T default_value) const;
};
```

#### Void Specialization
For operations that perform side effects without returning data:
```cpp
template <typename E>
class Result<void, E> {
public:
    [[nodiscard]] bool is_ok() const noexcept;
    [[nodiscard]] bool is_err() const noexcept;
    [[nodiscard]] const E& error() const;
};
```

### 1.3 Usage Pattern
Functions return `Result` by value, allowing clean fail-fast checking at system boundaries:

```cpp
core::Result<IniDocument> doc_result = IniParser::parse_file("/mnt/us/launchpad/launchpad.ini");
if (doc_result.is_err()) {
    Logger::instance().error("Failed to parse config: " + doc_result.error().message);
    return;
}

const IniDocument& doc = doc_result.value();
// Process doc...
```

---

## 2. Strongly Typed Units & Value Objects (`units.hpp`)

Adhering to Object Calisthenics Rule 3 (*Wrap all primitives*), scalar types are wrapped in explicit value objects.

### 2.1 `Milliseconds`
Eliminates unit ambiguity across timeouts and polling delays:
```cpp
struct Milliseconds {
    uint32_t value{0};
    constexpr explicit Milliseconds(uint32_t ms = 0) noexcept : value(ms) {}
    constexpr auto operator<=>(const Milliseconds&) const noexcept = default;
};
```
Prevents accidental passing of raw seconds or microseconds into timeout APIs.

### 2.2 `RawKeyCode` & `KeySequence`
Linux evdev key codes are typed as `RawKeyCode`:
```cpp
using RawKeyCode = int16_t;
```

Sequences of keystrokes are encapsulated in `KeySequence`, a first-class collection (Object Calisthenics Rule 4):
```cpp
struct KeySequence {
    std::vector<RawKeyCode> codes;

    void push(RawKeyCode code);
    void clear() noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] size_t size() const noexcept;

    auto operator<=>(const KeySequence&) const noexcept = default;
};
```
Because `KeySequence` implements the three-way comparison operator (`operator<=>`), it can be stored directly as keys in associative containers (`std::map`, `std::unordered_map`).

---

## 3. Move-Only RAII File Descriptors (`file_descriptor.hpp`)

Linux system programming requires managing file descriptors for `/dev/input/event*`, `/dev/fb0`, and `/var/run/launchpad.pid`. Unclosed descriptors leak kernel resources and prevent device re-acquisition.

`ScopedFileDescriptor` encapsulates a POSIX descriptor with strict move-only semantics:

```cpp
class ScopedFileDescriptor {
public:
    constexpr explicit ScopedFileDescriptor(int fd) noexcept;
    ~ScopedFileDescriptor(); // Automatically calls ::close(fd_)

    ScopedFileDescriptor(const ScopedFileDescriptor&) = delete;
    ScopedFileDescriptor& operator=(const ScopedFileDescriptor&) = delete;

    ScopedFileDescriptor(ScopedFileDescriptor&& other) noexcept;
    ScopedFileDescriptor& operator=(ScopedFileDescriptor&& other) noexcept;

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] int get() const noexcept;
    int release() noexcept;
    void reset(int new_fd = -1) noexcept;
};
```

Guarantees:
- **No Resource Leaks**: Descriptors are closed when going out of scope, even during early returns or errors.
- **Single Ownership**: Deleted copy operations prevent double-close bugs.

---

## 4. Diagnostic Logging (`logger.hpp`)

Kindle Launchpad provides a lightweight logging facility supporting two output modes:

```cpp
enum class LogLevel { Debug, Info, Warning, Error };

class Logger {
public:
    static Logger& instance();
    void init(const std::string& log_file_path, bool log_to_file);
    void close();

    void log(LogLevel level, std::string_view message);
    void debug(std::string_view message);
    void info(std::string_view message);
    void warning(std::string_view message);
    void error(std::string_view message);
};
```

### Routing Logic
1. **Interactive / Debug Mode (`-d`)**: Logs formatted output directly to standard error (`stderr`) for real-time visibility in terminal sessions.
2. **Daemon Mode**: Logs output with UTC timestamps and severity tags to `launchpad.log` in the application directory.
