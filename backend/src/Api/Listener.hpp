#pragma once

#include "AppRouter.hpp"
#include "Core/Types.hpp"
#include "Types.hpp"

#include <functional>

namespace WebPtt::Api {
class Listener {
public:
    using WebSocketUpgradeCallback =
        std::function<void(Tcp::socket, Http::request<Http::string_body>)>;

    Listener(Tcp::acceptor acceptor, WebSocketUpgradeCallback on_websocket_upgrade);

    void listen();

private:
    Tcp::acceptor acceptor_;
    std::shared_ptr<AppRouter> router_;
    WebSocketUpgradeCallback on_websocket_upgrade_;
};
} // namespace WebPtt::Api
