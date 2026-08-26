#pragma once

#include "Types.hpp"
#include "Core/Types.hpp"
#include "boost/asio/any_io_executor.hpp"

#include <boost/beast/core/flat_buffer.hpp>
#include <deque>
#include <functional>
#include <string>

namespace WebPtt::Api {
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    using ConnectedCallback = std::function<void(const std::shared_ptr<WebSocketSession>&)>;
    using DisconnectCallback = std::function<void(const std::shared_ptr<WebSocketSession>&)>;

    explicit WebSocketSession(Tcp::socket socket);

    void start(
        Http::request<Http::string_body> request,
        std::string peer_id,
        ConnectedCallback on_connected,
        DisconnectCallback on_disconnected);
    void write(std::string message);

    [[nodiscard]] const std::string& id() const noexcept { return peer_id_; }
    [[nodiscard]] boost::asio::any_io_executor executor() noexcept { return stream_.get_executor(); }

private:
    void disconnect();
    void do_read();
    void do_write();

    WebSocket::stream<Tcp::socket> stream_;
    Tcp::endpoint remote_endpoint_;
    boost::beast::flat_buffer buffer_;
    std::deque<std::string> write_queue_;
    bool is_writing_{};
    bool is_disconnected_{};
    DisconnectCallback on_disconnected_;
    std::string peer_id_;
};
} // namespace WebPtt::Api
