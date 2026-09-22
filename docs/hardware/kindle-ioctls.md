# Kindle Kernel IOCTL Reference & Hardware Safety

This document details the Linux kernel `ioctl` system calls utilized by Kindle Launchpad to control input grabbing and e-ink display updates.

---

## 1. Input Subsystem IOCTL: `EVIOCGRAB`

Defined in `<linux/input.h>`, `EVIOCGRAB` controls exclusive event grabbing on Linux `evdev` nodes (`/dev/input/event*`).

### 1.1 Signature & Usage
```c
int ioctl(int fd, EVIOCGRAB, int grab_flag);
```

- **`fd`**: Open file descriptor to an input event node (e.g. `/dev/input/event0`).
- **`grab_flag = 1`**: Acquires exclusive ownership of the input event stream. Subsequent hardware events are delivered **only** to the calling file descriptor.
- **`grab_flag = 0`**: Releases exclusive ownership. Subsequent events are broadcast to all listening processes.

### 1.2 Return Codes & Error Handling
- **`0`**: Success. Grab asserted or released.
- **`-1` with `EBUSY`**: Another process has already asserted `EVIOCGRAB` on this node.
- **`-1` with `EBADF`**: Invalid file descriptor or device node disconnected.

### 1.3 Safety & Recovery Guidelines
- **Always Pair with RAII**: Input grabbing must always be wrapped in an RAII object (`hal::ScopedInputGrab`). If an unhandled exception or early exit occurs while grabbed, the device keyboard would become permanently non-responsive to the user.
- **Signal Handling**: If Launchpad receives `SIGINT` or `SIGTERM`, the signal handler immediately ungrabs all descriptors before terminating.

---

## 2. E-Ink Framebuffer IOCTL: `FBIO_EINK_UPDATE_DISPLAY_AREA`

Amazon's custom Linux kernel provides an e-ink update ioctl to trigger screen refreshes on `/dev/fb0`.

### 2.1 IOCTL Definition & Data Structures
```c
#define FBIO_EINK_UPDATE_DISPLAY_AREA 0x46DD

struct update_area_t {
    int x1;           /* Left pixel coordinate */
    int y1;           /* Top pixel coordinate */
    int x2;           /* Right pixel coordinate */
    int y2;           /* Bottom pixel coordinate */
    int which_fx;     /* Flash effect / waveform mode */
    int buffer_id;    /* Framebuffer buffer index (usually 0) */
};
```

### 2.2 Waveform Modes (`which_fx`)

| Mode Flag | Constant Value | Waveform Type | Visual Effect & Use Case |
| :--- | :--- | :--- | :--- |
| **Partial / Fast** | `0` (`fx_update_partial`) | DU (Direct Update) / GLR16 | Fast non-flashing update. Used for status bar popups, text typing feedback, and cursor movement. Minimal latency (~150 ms). |
| **Full Refresh** | `1` (`fx_update_full`) | GC16 (Global Clear) | Inverts the screen to black, then white, then draws content. Clears e-ink ghosting and residual pigment particles. Higher latency (~600 ms). |

---

## 3. Implementation in `KindleDisplay::update()`

In `src/hal/kindle_display.cpp`:

```cpp
core::Result<void> KindleDisplay::update(const UpdateRegion& region) {
    if (!handle_.is_mmapped || handle_.file_descriptor < 0) {
        return core::Error("Framebuffer not open for update");
    }

    struct {
        int x1;
        int y1;
        int x2;
        int y2;
        int which_fx;
        int buffer_id;
    } area = {
        .x1 = static_cast<int>(region.x),
        .y1 = static_cast<int>(region.y),
        .x2 = static_cast<int>(region.x + region.width),
        .y2 = static_cast<int>(region.y + region.height),
        .which_fx = 0, // Partial refresh (fast, no black flash)
        .buffer_id = 0
    };

    int result = ::ioctl(handle_.file_descriptor, 0x46DD, &area);
    if (result < 0) {
        return core::Error("FBIO_EINK_UPDATE_DISPLAY_AREA ioctl failed");
    }

    return core::Result<void>();
}
```

This guarantees fast, non-disruptive visual notifications on e-ink hardware.
