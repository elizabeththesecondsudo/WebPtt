#include "Session.hpp"

#include <boost/asio/socket_base.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include <spdlog/spdlog.h>

namespace MediaPlane::Api {
Session::Session(Tcp::socket socket, std::shared_ptr<AppRouter> router)
    : socket_(std::move(socket))
    , remote_endpoint_(socket_.remote_endpoint())
    , router_(std::move(router)) {}

void Session::start() {
    do_read();
}

void Session::do_read() {
    request_ = {};

    Http::async_read(
        socket_,
        buffer_,
        request_,
        [self = shared_from_this()](boost::system::error_code error, std::size_t) {
            if (error == Http::error::end_of_stream) {
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

            auto response = self->router_->route(self->request_);
            self->do_write(std::move(response));
        });
}

void Session::do_write(Http::response<Http::string_body> response) {
    response_ = std::move(response);

    Http::async_write(socket_, response_, [self = shared_from_this()](boost::system::error_code error, std::size_t) {
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
} // namespace MediaPlane::Api
