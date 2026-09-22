/**
 * @file evdev_input_source.hpp
 * @brief Concrete Linux evdev input source implementation with EVIOCGRAB support.
 * @details Opens and monitors multiple /dev/input/event* nodes using POSIX poll(),
 *          filtering input_event structures and managing kernel input grabbing.
 */

#pragma once

#include "launchpad/hal/input_source.hpp"
#include <string>
#include <vector>

namespace launchpad::hal {

/**
 * @brief Aggregate managing open Linux file descriptors and grab state.
 */
struct DeviceGroup {
    std::vector<int> descriptors; ///< Open file descriptors for event devices.
    bool grabbed{false};          ///< Whether EVIOCGRAB is actively asserted on descriptors.
};

/**
 * @brief Configuration data specifying filesystem paths to event nodes.
 */
struct DeviceConfiguration {
    std::vector<std::string> paths; ///< List of device paths (e.g. /dev/input/event0, event1).
};

/**
 * @brief Concrete IInputSource driver reading hardware events from Linux evdev nodes.
 */
class EvdevInputSource : public IInputSource {
public:
    /**
     * @brief Constructs an EvdevInputSource monitoring the specified device nodes.
     * @param[in] device_paths List of /dev/input/event* device paths to open.
     */
    explicit EvdevInputSource(std::vector<std::string> device_paths);

    /**
     * @brief Destructor closing open descriptors and releasing active grabs.
     */
    ~EvdevInputSource() override;

    EvdevInputSource(const EvdevInputSource&) = delete;
    EvdevInputSource& operator=(const EvdevInputSource&) = delete;

    /**
     * @brief Asserts EVIOCGRAB ioctl across all monitored event device descriptors.
     * @return Result<void> indicating whether grab succeeded on all devices.
     */
    core::Result<void> grab() override;

    /**
     * @brief De-asserts EVIOCGRAB ioctl across all monitored event device descriptors.
     * @return Result<void> indicating whether release succeeded.
     */
    core::Result<void> release() override;

    /**
     * @brief Checks whether the devices are currently grabbed.
     * @return True if grabbed, false otherwise.
     */
    [[nodiscard]] bool is_grabbed() const noexcept override;

    /**
     * @brief Polls monitored descriptors for input_event data using POSIX poll().
     * @param[in] timeout Maximum wait duration in milliseconds.
     * @return Vector of parsed InputKeyEvent records.
     */
    std::vector<InputKeyEvent> poll_events(core::Milliseconds timeout) override;

private:
    DeviceGroup group_;
    DeviceConfiguration config_;

    void open_devices();
    void close_devices();
    void read_events_from_fd(int fd, std::vector<InputKeyEvent>& events);
};

} // namespace launchpad::hal
