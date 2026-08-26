#pragma once

#include <cstddef>
#include <glaze/core/reflect.hpp>
#include <string>
#include <variant>

namespace WebPtt::Stt {
struct TranscribeResponse {
    std::string raw_text_;
    std::string command_;
    std::variant<double, std::string> value_;
    double processing_time_ms_{};
    double audio_duration_ms_{};
    std::size_t input_samples_{};
    std::size_t whisper_samples_{};
};

struct TranscribeErrorResponseDetails {
    std::string error_;
    std::string raw_text_;
    double processing_time_ms_{};
};

struct TranscribeErrorResponse {
    TranscribeErrorResponseDetails detail; // NOLINT
};

using TranscribeError = std::variant<std::string, TranscribeErrorResponse>;
} // namespace WebPtt::Stt
