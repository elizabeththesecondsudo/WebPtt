#pragma once

#include "Types.hpp"
#include <expected>

namespace WebPtt::Api {
std::expected<Tcp::endpoint, std::string> parse_endpoint(std::string_view address, uint16_t port);
std::expected<Tcp::acceptor, std::string> create_acceptor(const Executor& executor, const Tcp::endpoint& endpoint);
} // namespace WebPtt::Api