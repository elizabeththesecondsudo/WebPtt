#include "Settings.hpp"
#include <expected>
#include <glaze/core/reflect.hpp>
#include <glaze/toml/read.hpp>

namespace WebPtt::Core {
std::expected<Settings, std::string> load_settings(const std::string& filename) {
    Settings settings;
    std::string data;
    auto errc = glz::read_file_toml(settings, filename, data);
    if (errc) {
        return std::unexpected(glz::format_error(errc, data));
    }

    return settings;
}
} // namespace WebPtt::Core
