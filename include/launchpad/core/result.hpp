/**
 * @file result.hpp
 * @brief Zero-exception error handling via monadic Result<T, E> sum-type.
 * @details Provides a lightweight, typesafe replacement for C++ exceptions suited for
 *          embedded Linux environments (ARMv6, glibc 2.5) where exceptions incur
 *          undesirable code bloat and runtime overhead.
 */

#pragma once

#include <string>
#include <variant>
#include <utility>

namespace launchpad::core {

/**
 * @brief Represents an operation failure containing a descriptive diagnostic message.
 */
struct Error {
    std::string message; ///< Human-readable description of the failure reason.

    /**
     * @brief Constructs an Error with the given diagnostic message.
     * @param[in] msg Diagnostic description of the failure.
     */
    explicit Error(std::string msg) : message(std::move(msg)) {}
};

/**
 * @brief A value-or-error container representing either success (T) or failure (E).
 * @tparam T Type of the success value.
 * @tparam E Type of the error value, defaulting to core::Error.
 */
template <typename T, typename E = Error>
class Result {
public:
    /**
     * @brief Constructs a successful Result by copying the value.
     * @param[in] val Value to copy into the result.
     */
    Result(const T& val) : data_(val) {}

    /**
     * @brief Constructs a successful Result by moving the value.
     * @param[in] val Value to move into the result.
     */
    Result(T&& val) : data_(std::move(val)) {}

    /**
     * @brief Constructs an error Result by copying the error.
     * @param[in] err Error to copy into the result.
     */
    Result(const E& err) : data_(err) {}

    /**
     * @brief Constructs an error Result by moving the error.
     * @param[in] err Error to move into the result.
     */
    Result(E&& err) : data_(std::move(err)) {}

    /**
     * @brief Checks whether the result holds a success value.
     * @return True if successful, false if holding an error.
     */
    [[nodiscard]] bool is_ok() const noexcept {
        return std::holds_alternative<T>(data_);
    }

    /**
     * @brief Checks whether the result holds an error.
     * @return True if holding an error, false if successful.
     */
    [[nodiscard]] bool is_err() const noexcept {
        return std::holds_alternative<E>(data_);
    }

    /**
     * @brief Contextual boolean conversion checking for success.
     * @return True if successful, false otherwise.
     */
    explicit operator bool() const noexcept {
        return is_ok();
    }

    /**
     * @brief Accesses the success value (const reference).
     * @return Reference to the held value T.
     * @note Precondition: is_ok() must be true; otherwise throws std::bad_variant_access.
     */
    [[nodiscard]] const T& value() const {
        return std::get<T>(data_);
    }

    /**
     * @brief Accesses the success value (mutable reference).
     * @return Mutable reference to the held value T.
     * @note Precondition: is_ok() must be true; otherwise throws std::bad_variant_access.
     */
    [[nodiscard]] T& value() {
        return std::get<T>(data_);
    }

    /**
     * @brief Accesses the error value (const reference).
     * @return Reference to the held error E.
     * @note Precondition: is_err() must be true; otherwise throws std::bad_variant_access.
     */
    [[nodiscard]] const E& error() const {
        return std::get<E>(data_);
    }

    /**
     * @brief Accesses the error value (mutable reference).
     * @return Mutable reference to the held error E.
     * @note Precondition: is_err() must be true; otherwise throws std::bad_variant_access.
     */
    [[nodiscard]] E& error() {
        return std::get<E>(data_);
    }

    /**
     * @brief Returns the held value if successful, or a fallback default value if error.
     * @param[in] default_value Fallback value returned on failure.
     * @return Held value or default_value.
     */
    [[nodiscard]] T value_or(T default_value) const {
        if (is_ok()) {
            return std::get<T>(data_);
        }
        return default_value;
    }

private:
    std::variant<T, E> data_;
};

/**
 * @brief Specialization of Result for operations returning void on success.
 * @tparam E Type of the error value, defaulting to core::Error.
 */
template <typename E>
class Result<void, E> {
public:
    /**
     * @brief Constructs a successful void Result.
     */
    Result() : has_error_(false) {}

    /**
     * @brief Constructs an error void Result by copying the error.
     * @param[in] err Error to copy.
     */
    Result(const E& err) : error_(err), has_error_(true) {}

    /**
     * @brief Constructs an error void Result by moving the error.
     * @param[in] err Error to move.
     */
    Result(E&& err) : error_(std::move(err)), has_error_(true) {}

    /**
     * @brief Checks whether the operation succeeded.
     * @return True if successful, false if error.
     */
    [[nodiscard]] bool is_ok() const noexcept {
        return !has_error_;
    }

    /**
     * @brief Checks whether the operation failed.
     * @return True if error, false if successful.
     */
    [[nodiscard]] bool is_err() const noexcept {
        return has_error_;
    }

    /**
     * @brief Contextual boolean conversion checking for success.
     * @return True if successful, false otherwise.
     */
    explicit operator bool() const noexcept {
        return is_ok();
    }

    /**
     * @brief Accesses the error value.
     * @return Const reference to the error.
     */
    [[nodiscard]] const E& error() const {
        return error_;
    }

private:
    E error_{""};
    bool has_error_{false};
};

} // namespace launchpad::core
