#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/core/units.hpp"
#include "launchpad/domain/sequence_matcher.hpp"
#include <vector>

namespace launchpad::hal {

struct InputKeyEvent {
    core::RawKeyCode code{0};
    domain::KeyEventType type{domain::KeyEventType::Released};
};

class IInputSource {
public:
    virtual ~IInputSource() = default;

    virtual core::Result<void> grab() = 0;
    virtual core::Result<void> release() = 0;
    [[nodiscard]] virtual bool is_grabbed() const noexcept = 0;
    virtual std::vector<InputKeyEvent> poll_events(core::Milliseconds timeout) = 0;
};

class ScopedInputGrab {
public:
    explicit ScopedInputGrab(IInputSource& source) : source_(source) {
        if (source_.grab().is_ok()) {
            grabbed_ = true;
        }
    }

    ~ScopedInputGrab() {
        if (grabbed_) {
            static_cast<void>(source_.release());
        }
    }

    ScopedInputGrab(const ScopedInputGrab&) = delete;
    ScopedInputGrab& operator=(const ScopedInputGrab&) = delete;
    ScopedInputGrab(ScopedInputGrab&&) = delete;
    ScopedInputGrab& operator=(ScopedInputGrab&&) = delete;

    [[nodiscard]] bool is_grabbed() const noexcept { return grabbed_; }

private:
    IInputSource& source_;
    bool grabbed_{false};
};

} // namespace launchpad::hal
