#include "WebSocketSession.hpp"

#include "Messages.hpp"
#include "Utils/Json.hpp"

#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::Api {
WebSocketSession::WebSocketSession(Tcp::socket socket)
    : stream_(std::move(socket))
    , remote_endpoint_(stream_.next_layer().remote_endpoint()) {}

void WebSocketSession::start(Http::request<Http::string_body> request) {
    auto request_ptr = std::make_shared<Http::request<Http::string_body>>(std::move(request));
    stream_.async_accept(*request_ptr, [self = shared_from_this(), request_ptr](boost::system::error_code error) {
        if (error) {
            spdlog::error("Error accepting WebSocket connection: {}", error.message());
            return;
        }

        spdlog::info(
            "Upgraded connection from {}:{} to WebSocket",
            self->remote_endpoint_.address().to_string(),
            self->remote_endpoint_.port());
            
        self->webrtc_session_ = std::make_shared<WebRtc::Session>();

        auto message = Utils::serialize_json(
            SessionCreatedMessage{
                .session_id_ = self->webrtc_session_->id(),
            });

        if (!message) {
            spdlog::error("Failed to serialize session creation message: {}", message.error());
            return;
        }

        self->write(std::move(message.value()));
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
        if (error == boost::beast::websocket::error::closed) {
            spdlog::info(
                "WebSocket client {}:{} closed the connection",
                self->remote_endpoint_.address().to_string(),
                self->remote_endpoint_.port());
            return;
        }

        if (error) {
            spdlog::error("Error reading WebSocket message: {}", error.message());
            return;
        }

        auto message = boost::beast::buffers_to_string(self->buffer_.data());
        spdlog::info("Got from websocket: {}", message);
        self->buffer_.consume(self->buffer_.size());

        self->do_read();
    });
}

void WebSocketSession::do_write() {
    stream_.text(true);
    stream_.async_write(
        boost::asio::buffer(write_queue_.front()),
        [self = shared_from_this()](boost::system::error_code error, std::size_t) {
            if (error) {
                spdlog::error("Error writing WebSocket message: {}", error.message());
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
} // namespace WebPtt::Api
