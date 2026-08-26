#pragma once

#include "Types.hpp"
#include "Core/Types.hpp"

#include <boost/beast/core/flat_buffer.hpp>
#include <deque>
#include <string>

namespace WebPtt::Api {
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    explicit WebSocketSession(Tcp::socket socket);

    void start(Http::request<Http::string_body> request);
    void write(std::string message);

private:
    void do_read();
    void do_write();

    WebSocket::stream<Tcp::socket> stream_;
    Tcp::endpoint remote_endpoint_;
    boost::beast::flat_buffer buffer_;
    std::deque<std::string> write_queue_;
    bool is_writing_{};
};
} // namespace WebPtt::Api
