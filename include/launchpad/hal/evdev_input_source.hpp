#pragma once

#include "launchpad/hal/input_source.hpp"
#include <string>
#include <vector>

namespace launchpad::hal {

struct DeviceGroup {
    std::vector<int> descriptors;
    bool grabbed{false};
};

struct DeviceConfiguration {
    std::vector<std::string> paths;
};

class EvdevInputSource : public IInputSource {
public:
    explicit EvdevInputSource(std::vector<std::string> device_paths);
    ~EvdevInputSource() override;

    EvdevInputSource(const EvdevInputSource&) = delete;
    EvdevInputSource& operator=(const EvdevInputSource&) = delete;

    core::Result<void> grab() override;
    core::Result<void> release() override;
    [[nodiscard]] bool is_grabbed() const noexcept override;
    std::vector<InputKeyEvent> poll_events(core::Milliseconds timeout) override;

private:
    DeviceGroup group_;
    DeviceConfiguration config_;

    void open_devices();
    void close_devices();
    void read_events_from_fd(int fd, std::vector<InputKeyEvent>& events);
};

} // namespace launchpad::hal
