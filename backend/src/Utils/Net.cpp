#include "Net.hpp"

namespace WebPtt::Utils {
std::expected<Tcp::endpoint, std::string> parse_endpoint(std::string_view address, uint16_t port) {
    boost::system::error_code error;
    auto parsed_address = boost::asio::ip::make_address(address, error);
    if (error) {
        return std::unexpected(error.message());
    }

    return Tcp::endpoint(parsed_address, port);
}

std::expected<Tcp::socket, std::string>
create_socket(const Executor& executor, std::string_view address, uint16_t port) {
    auto endpoint_res = parse_endpoint(address, port);
    if (!endpoint_res) {
        return std::unexpected(endpoint_res.error());
    }

    Tcp::socket socket(executor);
    boost::system::error_code error;
    auto endpoint = endpoint_res.value();
    error = socket.connect(endpoint, error);
    if (error) {
        return std::unexpected(error.message());
    }

    return socket;
}

std::expected<Tcp::acceptor, std::string> create_acceptor(const Executor& executor, const Tcp::endpoint& endpoint) {
    Tcp::acceptor acceptor(executor);
    boost::system::error_code error;

    error = acceptor.open(endpoint.protocol(), error);
    if (error) {
        return std::unexpected(error.message());
    }

    error = acceptor.bind(endpoint, error);
    if (error) {
        return std::unexpected(error.message());
    }

    error = acceptor.listen(boost::asio::socket_base::max_listen_connections, error);
    if (error) {
        return std::unexpected(error.message());
    }

    return acceptor;
}

std::expected<Tcp::acceptor, std::string>
create_acceptor(const Executor& executor, std::string_view address, uint16_t port) {
    auto endpoint_res = parse_endpoint(address, port);
    if (!endpoint_res) {
        return std::unexpected(endpoint_res.error());
    }

    auto endpoint = std::move(endpoint_res.value());
    return create_acceptor(executor, endpoint);
}
} // namespace WebPtt::Utils
