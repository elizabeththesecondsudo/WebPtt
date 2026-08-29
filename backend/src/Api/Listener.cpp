#include "Listener.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include "Session.hpp"

namespace WebPtt::Api {
Listener::Listener(
    Tcp::acceptor acceptor,
    WebSocketUpgradeCallback on_websocket_upgrade,
    std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager)
    : acceptor_(std::move(acceptor))
    , router_(std::make_shared<AppRouter>(std::move(peer_connection_manager)))
    , on_websocket_upgrade_(std::move(on_websocket_upgrade)) {
    router_->register_routes();
    const auto endpoint = acceptor_.local_endpoint();
    spdlog::info("API listener initialized on {}:{}", endpoint.address().to_string(), endpoint.port());
}

void Listener::listen() {
    acceptor_.async_accept([this](boost::system::error_code error, Tcp::socket socket) {
        if (error) {
            spdlog::error("Error accepting connection: {}", error.message());
            return;
        }

        spdlog::info(
            "Got connection from: {}:{}",
            socket.remote_endpoint().address().to_string(),
            socket.remote_endpoint().port());

        std::make_shared<Session>(std::move(socket), router_)->start(on_websocket_upgrade_);
        listen(); // Continue accepting new connections
    });
}
} // namespace WebPtt::Api
