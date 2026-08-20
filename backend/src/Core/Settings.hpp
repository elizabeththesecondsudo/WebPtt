#pragma once

#include <cstdint>
#include <expected>
#include <string>

namespace WebPtt::Core {
struct MediaPlane {
    std::string listening_address_;
    uint16_t listening_port_{};
};

struct Stt {
    std::string model_path_;
};

struct Settings {
    MediaPlane media_plane_;
    Stt stt_;
};

std::expected<Settings, std::string> load_settings(const std::string& filename);

} // namespace WebPtt::Core
