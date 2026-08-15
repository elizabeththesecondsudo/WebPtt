#pragma once

#include <cstdint>
#include <expected>
#include <string>

namespace WebPtt::Core {
struct FrontendContext {
    std::string address_;
    uint16_t port_{};
};

struct Settings {
    FrontendContext frontend_context_;
};

std::expected<Settings, std::string> load_settings(const std::string& filename);

} // namespace WebPtt::Core
