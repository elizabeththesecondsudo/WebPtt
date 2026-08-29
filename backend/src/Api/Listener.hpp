#pragma once

#include "AppRouter.hpp"
#include "Core/Types.hpp"
#include "Types.hpp"

#include <functional>

namespace WebPtt::Api {
namespace WebRtc = WebPtt::WebRtc;
class Listener {
public:
    using WebSocketUpgradeCallback = std::function<void(Tcp::socket, Http::request<Http::string_body>)>;

    Listener(
        Tcp::acceptor acceptor,
        WebSocketUpgradeCallback on_websocket_upgrade,
        std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager);

    void listen();

private:
    Tcp::acceptor acceptor_;
    std::shared_ptr<AppRouter> router_;
    WebSocketUpgradeCallback on_websocket_upgrade_;
};
} // namespace WebPtt::Api
