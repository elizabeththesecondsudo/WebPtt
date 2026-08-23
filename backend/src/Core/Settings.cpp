#include "Settings.hpp"
#include <expected>
#include "Utils/Toml.hpp"

namespace WebPtt::Core {
std::expected<Settings, std::string> load_settings(const std::string& filename) {
    return Utils::parse_toml_file<Settings>(filename);
}
} // namespace WebPtt::Core
