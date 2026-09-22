#include "launchpad/system/daemon.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdlib>

namespace launchpad::system {

core::Result<void> Daemon::fork_process() {
    pid_t pid = fork();
    if (pid < 0) {
        return core::Result<void>(core::Error("Failed to fork process"));
    }
    if (pid > 0) {
        _exit(EXIT_SUCCESS);
    }
    return core::Result<void>();
}

core::Result<void> Daemon::setup_session() {
    if (setsid() < 0) {
        return core::Result<void>(core::Error("Failed to create new session"));
    }
    umask(0022);
    return core::Result<void>();
}

void Daemon::redirect_standard_streams(std::string_view log_path) {
    int null_fd = open("/dev/null", O_RDONLY);
    int log_fd = open(std::string(log_path).c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        log_fd = open("/dev/null", O_WRONLY);
    }
    dup2(null_fd, STDIN_FILENO);
    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
    if (null_fd > STDERR_FILENO) close(null_fd);
    if (log_fd > STDERR_FILENO) close(log_fd);
}

core::Result<void> Daemon::daemonize(std::string_view log_path) {
    if (getppid() == 1) {
        return core::Result<void>();
    }
    auto fork_res = fork_process();
    if (fork_res.is_err()) {
        return fork_res;
    }
    auto session_res = setup_session();
    if (session_res.is_err()) {
        return session_res;
    }
    redirect_standard_streams(log_path);
    return core::Result<void>();
}

} // namespace launchpad::system
