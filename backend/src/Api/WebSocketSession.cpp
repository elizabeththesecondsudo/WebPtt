#include "WebSocketSession.hpp"
#include <boost/asio/error.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>
#include <utility>

namespace {
bool is_websocket_closed(const boost::system::error_code& error) {
    return error == boost::beast::websocket::error::closed || error == boost::asio::error::eof;
}
} // namespace

namespace WebPtt::Api {
WebSocketSession::WebSocketSession(Tcp::socket socket)
    : stream_(std::move(socket))
    , remote_endpoint_(stream_.next_layer().remote_endpoint()) {}

void WebSocketSession::start(
    Http::request<Http::string_body> request,
    std::string peer_id,
    ConnectedCallback on_connected,
    DisconnectCallback on_disconnected) {
    on_disconnected_ = std::move(on_disconnected);
    peer_id_ = std::move(peer_id);
    auto request_ptr = std::make_shared<Http::request<Http::string_body>>(std::move(request));
    stream_.async_accept(
        *request_ptr,
        [self = shared_from_this(), request_ptr, on_connected = std::move(on_connected)](
            boost::system::error_code error) {
            if (is_websocket_closed(error)) {
                spdlog::info(
                    "WebSocket client {}:{} closed during handshake",
                    self->remote_endpoint_.address().to_string(),
                    self->remote_endpoint_.port());
                self->disconnect();
                return;
            }

            if (error) {
                spdlog::error("Error accepting WebSocket connection: {}", error.message());
                self->disconnect();
                return;
            }

            spdlog::info(
                "Upgraded connection from {}:{} to WebSocket",
                self->remote_endpoint_.address().to_string(),
                self->remote_endpoint_.port());

            if (on_connected) {
                on_connected(self);
            }
            self->do_read();
        });
}

void WebSocketSession::write(std::string message) {
    write_queue_.push_back(std::move(message));
    if (is_writing_) {
        return;
    }

    is_writing_ = true;
    do_write();
}

void WebSocketSession::do_read() {
    stream_.async_read(buffer_, [self = shared_from_this()](boost::system::error_code error, std::size_t) {
        if (is_websocket_closed(error)) {
            spdlog::info(
                "WebSocket client {}:{} closed the connection",
                self->remote_endpoint_.address().to_string(),
                self->remote_endpoint_.port());
            self->disconnect();
            return;
        }

        if (error) {
            spdlog::error("Error reading WebSocket message: {}", error.message());
            self->disconnect();
            return;
        }

        auto message = boost::beast::buffers_to_string(self->buffer_.data());
        spdlog::debug(
            "Received {} bytes from WebSocket client {}:{}",
            message.size(),
            self->remote_endpoint_.address().to_string(),
            self->remote_endpoint_.port());
        self->buffer_.consume(self->buffer_.size());

        self->do_read();
    });
}

void WebSocketSession::do_write() {
    stream_.text(true);
    stream_.async_write(
        boost::asio::buffer(write_queue_.front()),
        [self = shared_from_this()](boost::system::error_code error, std::size_t) {
            if (is_websocket_closed(error)) {
                spdlog::info(
                    "WebSocket client {}:{} closed the connection",
                    self->remote_endpoint_.address().to_string(),
                    self->remote_endpoint_.port());
                self->disconnect();
                return;
            }

            if (error) {
                spdlog::error("Error writing WebSocket message: {}", error.message());
                self->disconnect();
                return;
            }

            self->write_queue_.pop_front();
            if (!self->write_queue_.empty()) {
                self->do_write();
                return;
            }

            self->is_writing_ = false;
        });
}

void WebSocketSession::disconnect() {
    if (is_disconnected_) {
        return;
    }

    is_disconnected_ = true;
    if (on_disconnected_) {
        on_disconnected_(shared_from_this());
    }
}
} // namespace WebPtt::Api
