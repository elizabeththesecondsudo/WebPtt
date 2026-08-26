#include "Session.hpp"

#include <boost/asio/error.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <spdlog/spdlog.h>

namespace {
bool is_http_end_of_stream(const boost::system::error_code& error) {
    return error == WebPtt::Api::Http::error::end_of_stream || error == boost::asio::error::eof;
}
} // namespace

namespace WebPtt::Api {
Session::Session(Tcp::socket socket, std::shared_ptr<AppRouter> router)
    : socket_(std::move(socket))
    , remote_endpoint_(socket_.remote_endpoint())
    , router_(std::move(router)) {}

void Session::start(WebSocketUpgradeCallback on_websocket_upgrade) {
    on_websocket_upgrade_ = std::move(on_websocket_upgrade);
    do_read();
}

void Session::do_read() {
    request_ = {};

    Http::async_read(
        socket_,
        buffer_,
        request_,
        [self = shared_from_this()](boost::system::error_code error, std::size_t) {
            if (is_http_end_of_stream(error)) {
                spdlog::info(
                    "HTTP client {}:{} closed the connection",
                    self->remote_endpoint_.address().to_string(),
                    self->remote_endpoint_.port());
                boost::system::error_code shutdown_error;
                shutdown_error = self->socket_.shutdown(Tcp::socket::shutdown_send, shutdown_error);
                return;
            }

            if (error) {
                spdlog::error("Error reading HTTP request: {}", error.message());
                return;
            }

            if (WebSocket::is_upgrade(self->request_)) {
                self->on_websocket_upgrade_(std::move(self->socket_), std::move(self->request_));
                return;
            }

            auto response = self->router_->route(self->request_);
            self->do_write(std::move(response));
        });
}

void Session::do_write(Http::response<Http::string_body> response) {
    response_ = std::move(response);

    Http::async_write(socket_, response_, [self = shared_from_this()](boost::system::error_code error, std::size_t) {
        if (is_http_end_of_stream(error)) {
            spdlog::info(
                "HTTP client {}:{} closed the connection",
                self->remote_endpoint_.address().to_string(),
                self->remote_endpoint_.port());
            return;
        }

        if (error) {
            spdlog::error("Error writing HTTP response: {}", error.message());
            return;
        }

        if (self->response_.keep_alive()) {
            self->do_read();
            return;
        }

        boost::system::error_code shutdown_error;
        shutdown_error = self->socket_.shutdown(Tcp::socket::shutdown_send, shutdown_error);
    });
}
} // namespace WebPtt::Api
