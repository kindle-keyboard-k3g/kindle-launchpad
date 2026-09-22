#pragma once

#include "launchpad/core/result.hpp"
#include <string>
#include <string_view>

namespace launchpad::system {

class PidLock {
public:
    explicit PidLock(std::string_view lock_path) noexcept;
    ~PidLock() noexcept;

    PidLock(const PidLock&) = delete;
    PidLock& operator=(const PidLock&) = delete;
    PidLock(PidLock&& other) noexcept;
    PidLock& operator=(PidLock&& other) noexcept;

    [[nodiscard]] core::Result<void> acquire();
    void release() noexcept;
    [[nodiscard]] bool is_locked() const noexcept;

private:
    std::string lock_path_;
    int file_descriptor_{-1};

    void write_pid_to_file();
};

} // namespace launchpad::system
