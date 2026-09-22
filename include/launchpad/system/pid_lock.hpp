/**
 * @file pid_lock.hpp
 * @brief PID file lock manager using non-blocking flock().
 * @details Prevents concurrent daemon instances by acquiring an advisory lock
 *          (LOCK_EX | LOCK_NB) on /var/run/launchpad.pid and recording the process ID.
 */

#pragma once

#include "launchpad/core/result.hpp"
#include <string>
#include <string_view>

namespace launchpad::system {

/**
 * @brief RAII manager for daemon process lockfiles.
 */
class PidLock {
public:
    /**
     * @brief Constructs a PidLock targeting the specified lockfile path.
     * @param[in] lock_path Path to the PID lockfile (e.g. /var/run/launchpad.pid).
     */
    explicit PidLock(std::string_view lock_path) noexcept;

    /**
     * @brief Destructor releasing the lock and removing the lockfile.
     */
    ~PidLock() noexcept;

    PidLock(const PidLock&) = delete;
    PidLock& operator=(const PidLock&) = delete;

    /**
     * @brief Move constructor transferring lock ownership.
     * @param[in,out] other Source lock instance.
     */
    PidLock(PidLock&& other) noexcept;

    /**
     * @brief Move assignment transferring lock ownership.
     * @param[in,out] other Source lock instance.
     * @return Reference to this instance.
     */
    PidLock& operator=(PidLock&& other) noexcept;

    /**
     * @brief Opens and acquires a non-blocking exclusive flock on the lockfile.
     * @return Result<void> indicating success, or an Error if already locked by another process.
     */
    [[nodiscard]] core::Result<void> acquire();

    /**
     * @brief Releases the lock and unlinks the lockfile from the filesystem.
     */
    void release() noexcept;

    /**
     * @brief Checks if the lock is currently held by this instance.
     * @return True if locked, false otherwise.
     */
    [[nodiscard]] bool is_locked() const noexcept;

private:
    std::string lock_path_;
    int file_descriptor_{-1};

    void write_pid_to_file();
};

} // namespace launchpad::system
