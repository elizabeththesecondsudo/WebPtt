#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <expected>
#include "Core/Types.hpp"

namespace WebPtt::Utils {
std::expected<boost::asio::ip::tcp::endpoint, std::string> parse_endpoint(std::string_view address, uint16_t port);

std::expected<boost::asio::ip::tcp::socket, std::string>
create_socket(const Executor& executor, std::string_view address, uint16_t port);

std::expected<Tcp::acceptor, std::string> create_acceptor(const Executor& executor, const Tcp::endpoint& endpoint);

std::expected<Tcp::acceptor, std::string>
create_acceptor(const Executor& executor, std::string_view address, uint16_t port);
} // namespace WebPtt::Utils