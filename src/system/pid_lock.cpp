#include "launchpad/system/pid_lock.hpp"
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>

namespace launchpad::system {

PidLock::PidLock(std::string_view lock_path) noexcept
    : lock_path_(lock_path) {}

PidLock::~PidLock() noexcept {
    release();
}

PidLock::PidLock(PidLock&& other) noexcept
    : lock_path_(std::move(other.lock_path_)),
      file_descriptor_(std::exchange(other.file_descriptor_, -1)) {}

PidLock& PidLock::operator=(PidLock&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    release();
    lock_path_ = std::move(other.lock_path_);
    file_descriptor_ = std::exchange(other.file_descriptor_, -1);
    return *this;
}

bool PidLock::is_locked() const noexcept {
    return file_descriptor_ != -1;
}

void PidLock::write_pid_to_file() {
    auto truncated = ftruncate(file_descriptor_, 0);
    (void)truncated;
    dprintf(file_descriptor_, "%d\n", getpid());
}

core::Result<void> PidLock::acquire() {
    if (is_locked()) {
        return core::Result<void>();
    }
    file_descriptor_ = open(lock_path_.c_str(), O_RDWR | O_CREAT, 0640);
    if (file_descriptor_ < 0) {
        return core::Result<void>(core::Error("Could not open lock file"));
    }
    if (flock(file_descriptor_, LOCK_EX | LOCK_NB) != 0) {
        close(file_descriptor_);
        file_descriptor_ = -1;
        return core::Result<void>(core::Error("Lock already held by another process"));
    }
    write_pid_to_file();
    return core::Result<void>();
}

void PidLock::release() noexcept {
    if (!is_locked()) {
        return;
    }
    flock(file_descriptor_, LOCK_UN);
    close(file_descriptor_);
    unlink(lock_path_.c_str());
    file_descriptor_ = -1;
}

} // namespace launchpad::system
