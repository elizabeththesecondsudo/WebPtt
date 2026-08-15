#include "Utils.hpp"
#include "boost/beast/http/message_fwd.hpp"
#include "boost/beast/http/string_body_fwd.hpp"

namespace WebPtt::Api {
std::expected<Tcp::endpoint, std::string> parse_endpoint(std::string_view address, uint16_t port) {
    boost::system::error_code error;
    auto parsed_address = boost::asio::ip::make_address(address, error);
    if (error) {
        return std::unexpected(error.message());
    }

    return Tcp::endpoint(parsed_address, port);
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

Http::response<Http::string_body> make_basic_api_response(
    const Http::request<Http::string_body>& request,
    Http::status status) {
    Http::response<Http::string_body> response{status, request.version()};
    response.set(Http::field::server, "WebPtt");
    response.set(Http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());
    return response;
}
} // namespace WebPtt::Api
