#pragma once

#include "Types.hpp"
#include <expected>

namespace WebPtt::Api {
std::expected<Tcp::endpoint, std::string> parse_endpoint(std::string_view address, uint16_t port);
std::expected<Tcp::acceptor, std::string> create_acceptor(const Executor& executor, const Tcp::endpoint& endpoint);
Http::response<Http::string_body> make_basic_api_response(
    const Http::request<Http::string_body>& request,
    Http::status status);
} // namespace WebPtt::Api