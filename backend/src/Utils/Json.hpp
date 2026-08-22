#pragma once

#include <expected>
#include <glaze/json.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace WebPtt::Utils {
template <typename Struct>
std::expected<Struct, std::string> parse_json(std::string_view json) {
    auto result = glz::read_json<Struct>(json);
    if (!result) {
        return std::unexpected(glz::format_error(result.error(), json));
    }

    return std::move(result.value());
}

template <typename Struct>
std::expected<std::string, std::string> serialize_json(const Struct& value) {
    auto result = glz::write_json(value);
    if (!result) {
        return std::unexpected(glz::format_error(result.error()));
    }

    return std::move(result.value());
}
} // namespace WebPtt::Utils
