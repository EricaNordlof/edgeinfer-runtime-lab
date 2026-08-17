#pragma once
#include <string>
#include <utility>

namespace edgeinfer {
enum class StatusCode {
    Ok = 0,
    InvalidArgument,
    IoError,
    InvalidModel,
    BackendUnavailable,
    InferenceError
};

class Status {
public:
    Status() = default;
    Status(StatusCode code, std::string message)
        : code_(code), message_(std::move(message)) {}

    static Status ok() { return {}; }
    bool is_ok() const { return code_ == StatusCode::Ok; }
    StatusCode code() const { return code_; }
    const std::string& message() const { return message_; }

private:
    StatusCode code_ = StatusCode::Ok;
    std::string message_;
};
}
