/**
 * @file input_source.hpp
 * @brief Abstract input source interface and RAII input grab guard.
 * @details Declares IInputSource for event polling and exclusive hardware grabbing,
 *          plus ScopedInputGrab for deterministic acquisition and release of input streams.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/core/units.hpp"
#include "launchpad/domain/sequence_matcher.hpp"
#include <vector>

namespace launchpad::hal {

/**
 * @brief Single input event emitted by an input source device.
 */
struct InputKeyEvent {
    core::RawKeyCode code{0};                          ///< Raw Linux evdev key code.
    domain::KeyEventType type{domain::KeyEventType::Released}; ///< Key state transition (Pressed or Released).
};

/**
 * @brief Abstract hardware interface for polling key events and acquiring exclusive input grab.
 */
class IInputSource {
public:
    virtual ~IInputSource() = default;

    /**
     * @brief Acquires exclusive access to the hardware input devices (e.g. ioctl(EVIOCGRAB, 1)).
     * @return Result<void> indicating whether grab succeeded.
     */
    virtual core::Result<void> grab() = 0;

    /**
     * @brief Releases exclusive access to the hardware input devices (e.g. ioctl(EVIOCGRAB, 0)).
     * @return Result<void> indicating whether release succeeded.
     */
    virtual core::Result<void> release() = 0;

    /**
     * @brief Checks if exclusive input grab is currently active.
     * @return True if grabbed, false otherwise.
     */
    [[nodiscard]] virtual bool is_grabbed() const noexcept = 0;

    /**
     * @brief Polls underlying event sources for pending key events up to the given timeout.
     * @param[in] timeout Maximum duration to block waiting for input events.
     * @return Vector of captured InputKeyEvent instances.
     */
    virtual std::vector<InputKeyEvent> poll_events(core::Milliseconds timeout) = 0;
};

/**
 * @brief RAII guard managing exclusive input grab lifetime.
 * @details Automatically calls IInputSource::grab() on construction and IInputSource::release()
 *          on destruction. Non-copyable and non-movable.
 */
class ScopedInputGrab {
public:
    /**
     * @brief Constructs guard and attempts to acquire exclusive input grab on source.
     * @param[in,out] source IInputSource instance to grab.
     */
    explicit ScopedInputGrab(IInputSource& source) : source_(source) {
        if (source_.grab().is_ok()) {
            grabbed_ = true;
        }
    }

    /**
     * @brief Destructor releasing the input grab if previously acquired.
     */
    ~ScopedInputGrab() {
        if (grabbed_) {
            static_cast<void>(source_.release());
        }
    }

    ScopedInputGrab(const ScopedInputGrab&) = delete;
    ScopedInputGrab& operator=(const ScopedInputGrab&) = delete;
    ScopedInputGrab(ScopedInputGrab&&) = delete;
    ScopedInputGrab& operator=(ScopedInputGrab&&) = delete;

    /**
     * @brief Checks whether the input source was successfully grabbed.
     * @return True if grab is currently active.
     */
    [[nodiscard]] bool is_grabbed() const noexcept { return grabbed_; }

private:
    IInputSource& source_;
    bool grabbed_{false};
};

} // namespace launchpad::hal
