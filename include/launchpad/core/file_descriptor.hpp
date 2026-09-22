/**
 * @file file_descriptor.hpp
 * @brief Move-only RAII wrapper for POSIX file descriptors.
 * @details Ensures leak-free ownership of Linux file descriptors (evdev, framebuffer, pidfile)
 *          under normal operation, early returns, and unexpected exits.
 */

#pragma once

#include <unistd.h>
#include <utility>

namespace launchpad::core {

/**
 * @brief RAII guard managing the lifetime of an operating system file descriptor.
 * @details Non-copyable, move-constructible, and move-assignable. Automatically
 *          closes the underlying descriptor when leaving scope.
 */
class ScopedFileDescriptor {
public:
    /**
     * @brief Constructs an empty, invalid descriptor (-1).
     */
    constexpr ScopedFileDescriptor() noexcept : fd_(-1) {}

    /**
     * @brief Takes ownership of an existing raw file descriptor.
     * @param[in] fd Raw POSIX descriptor to wrap.
     */
    constexpr explicit ScopedFileDescriptor(int fd) noexcept : fd_(fd) {}

    /**
     * @brief Destructor closing the held descriptor if valid.
     */
    ~ScopedFileDescriptor() {
        reset();
    }

    ScopedFileDescriptor(const ScopedFileDescriptor&) = delete;
    ScopedFileDescriptor& operator=(const ScopedFileDescriptor&) = delete;

    /**
     * @brief Move constructor transferring ownership of the descriptor.
     * @param[in,out] other Source descriptor guard, reset to -1.
     */
    ScopedFileDescriptor(ScopedFileDescriptor&& other) noexcept : fd_(other.release()) {}

    /**
     * @brief Move assignment operator transferring ownership of the descriptor.
     * @param[in,out] other Source descriptor guard, reset to -1.
     * @return Reference to this guard.
     */
    ScopedFileDescriptor& operator=(ScopedFileDescriptor&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    /**
     * @brief Checks if the descriptor is valid (>= 0).
     * @return True if valid, false if uninitialized or closed (-1).
     */
    [[nodiscard]] bool is_valid() const noexcept {
        return fd_ >= 0;
    }

    /**
     * @brief Contextual boolean conversion checking descriptor validity.
     * @return True if is_valid(), false otherwise.
     */
    explicit operator bool() const noexcept {
        return is_valid();
    }

    /**
     * @brief Returns the raw file descriptor without transferring ownership.
     * @return Integer descriptor value.
     */
    [[nodiscard]] int get() const noexcept {
        return fd_;
    }

    /**
     * @brief Releases ownership of the file descriptor without closing it.
     * @return The raw descriptor value, transferring cleanup responsibility to the caller.
     */
    int release() noexcept {
        int old_fd = fd_;
        fd_ = -1;
        return old_fd;
    }

    /**
     * @brief Closes the currently held descriptor (if valid) and adopts a new descriptor.
     * @param[in] new_fd New descriptor to manage (defaults to -1).
     */
    void reset(int new_fd = -1) noexcept {
        if (fd_ >= 0) {
            ::close(fd_);
        }
        fd_ = new_fd;
    }

private:
    int fd_{-1}; ///< Managed POSIX file descriptor integer.
};

} // namespace launchpad::core
