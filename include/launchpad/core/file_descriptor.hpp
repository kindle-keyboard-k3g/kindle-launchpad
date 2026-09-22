#pragma once

#include <unistd.h>
#include <utility>

namespace launchpad::core {

class ScopedFileDescriptor {
public:
    constexpr ScopedFileDescriptor() noexcept : fd_(-1) {}
    constexpr explicit ScopedFileDescriptor(int fd) noexcept : fd_(fd) {}

    ~ScopedFileDescriptor() {
        reset();
    }

    ScopedFileDescriptor(const ScopedFileDescriptor&) = delete;
    ScopedFileDescriptor& operator=(const ScopedFileDescriptor&) = delete;

    ScopedFileDescriptor(ScopedFileDescriptor&& other) noexcept : fd_(other.release()) {}

    ScopedFileDescriptor& operator=(ScopedFileDescriptor&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return fd_ >= 0;
    }

    explicit operator bool() const noexcept {
        return is_valid();
    }

    [[nodiscard]] int get() const noexcept {
        return fd_;
    }

    int release() noexcept {
        int old_fd = fd_;
        fd_ = -1;
        return old_fd;
    }

    void reset(int new_fd = -1) noexcept {
        if (fd_ >= 0) {
            ::close(fd_);
        }
        fd_ = new_fd;
    }

private:
    int fd_{-1};
};

} // namespace launchpad::core
