#include "launchpad/hal/evdev_input_source.hpp"
#include <linux/input.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

namespace launchpad::hal {

EvdevInputSource::EvdevInputSource(std::vector<std::string> device_paths)
    : config_{std::move(device_paths)} {
    open_devices();
}

EvdevInputSource::~EvdevInputSource() {
    static_cast<void>(release());
    close_devices();
}

void EvdevInputSource::open_devices() {
    for (const auto& path : config_.paths) {
        int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            group_.descriptors.push_back(fd);
        }
    }
}

void EvdevInputSource::close_devices() {
    for (int fd : group_.descriptors) {
        close(fd);
    }
    group_.descriptors.clear();
}

core::Result<void> EvdevInputSource::grab() {
    for (int fd : group_.descriptors) {
        ioctl(fd, EVIOCGRAB, 1);
    }
    group_.grabbed = true;
    return core::Result<void>();
}

core::Result<void> EvdevInputSource::release() {
    if (!group_.grabbed) {
        return core::Result<void>();
    }
    for (int fd : group_.descriptors) {
        ioctl(fd, EVIOCGRAB, 0);
    }
    group_.grabbed = false;
    return core::Result<void>();
}

bool EvdevInputSource::is_grabbed() const noexcept {
    return group_.grabbed;
}

void EvdevInputSource::read_events_from_fd(int fd, std::vector<InputKeyEvent>& events) {
    input_event raw_events[8];
    ssize_t bytes = read(fd, raw_events, sizeof(raw_events));
    if (bytes < static_cast<ssize_t>(sizeof(input_event))) return;

    size_t count = static_cast<size_t>(bytes) / sizeof(input_event);
    for (size_t i = 0; i < count; ++i) {
        if (raw_events[i].type != EV_KEY) continue;
        if (raw_events[i].value == 2) continue; // ignore autorepeat

        auto type = (raw_events[i].value == 1)
            ? domain::KeyEventType::Pressed
            : domain::KeyEventType::Released;
        auto code = static_cast<core::RawKeyCode>(raw_events[i].code);
        events.push_back(InputKeyEvent{code, type});
    }
}

std::vector<InputKeyEvent> EvdevInputSource::poll_events(core::Milliseconds timeout) {
    std::vector<InputKeyEvent> events;
    std::vector<pollfd> poll_fds;
    poll_fds.reserve(group_.descriptors.size());
    for (int fd : group_.descriptors) {
        poll_fds.push_back(pollfd{fd, POLLIN, 0});
    }

    int rc = poll(poll_fds.data(), poll_fds.size(), static_cast<int>(timeout.value));
    if (rc <= 0) return events;

    for (const auto& pfd : poll_fds) {
        if (pfd.revents & POLLIN) {
            read_events_from_fd(pfd.fd, events);
        }
    }
    return events;
}

} // namespace launchpad::hal
