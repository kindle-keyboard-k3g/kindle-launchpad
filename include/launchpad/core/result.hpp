#pragma once

#include <string>
#include <variant>
#include <utility>

namespace launchpad::core {

struct Error {
    std::string message;

    explicit Error(std::string msg) : message(std::move(msg)) {}
};

template <typename T, typename E = Error>
class Result {
public:
    Result(const T& val) : data_(val) {}
    Result(T&& val) : data_(std::move(val)) {}
    Result(const E& err) : data_(err) {}
    Result(E&& err) : data_(std::move(err)) {}

    [[nodiscard]] bool is_ok() const noexcept {
        return std::holds_alternative<T>(data_);
    }

    [[nodiscard]] bool is_err() const noexcept {
        return std::holds_alternative<E>(data_);
    }

    explicit operator bool() const noexcept {
        return is_ok();
    }

    [[nodiscard]] const T& value() const {
        return std::get<T>(data_);
    }

    [[nodiscard]] T& value() {
        return std::get<T>(data_);
    }

    [[nodiscard]] const E& error() const {
        return std::get<E>(data_);
    }

    [[nodiscard]] E& error() {
        return std::get<E>(data_);
    }

    [[nodiscard]] T value_or(T default_value) const {
        if (is_ok()) {
            return std::get<T>(data_);
        }
        return default_value;
    }

private:
    std::variant<T, E> data_;
};

template <typename E>
class Result<void, E> {
public:
    Result() : has_error_(false) {}
    Result(const E& err) : error_(err), has_error_(true) {}
    Result(E&& err) : error_(std::move(err)), has_error_(true) {}

    [[nodiscard]] bool is_ok() const noexcept {
        return !has_error_;
    }

    [[nodiscard]] bool is_err() const noexcept {
        return has_error_;
    }

    explicit operator bool() const noexcept {
        return is_ok();
    }

    [[nodiscard]] const E& error() const {
        return error_;
    }

private:
    E error_{""};
    bool has_error_{false};
};

} // namespace launchpad::core
