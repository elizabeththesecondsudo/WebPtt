#pragma once

#include <expected>
#include <glaze/core/reflect.hpp>
#include <glaze/toml/read.hpp>
#include <string>

namespace WebPtt::Utils {
template <typename T>
std::expected<T, std::string> parse_toml_file(const std::string& filename) {
    T value{};
    std::string data;
    auto errc = glz::read_file_toml(value, filename, data);
    if (errc) {
        return std::unexpected(glz::format_error(errc, data));
    }

    return value;
}
} // namespace WebPtt::Utils
