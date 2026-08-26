#pragma once

#include "Api/Types.hpp"
#include "Types.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace WebPtt::Api {
class WebSocketManager;
class WebSocketSession;
} // namespace WebPtt::Api

namespace WebPtt::WebRtc {
class PeerConnectionManager;
}

namespace WebPtt::Core {
class Coordinator {
public:
    Coordinator(
        std::shared_ptr<Api::WebSocketManager> websocket_manager,
        std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager);

    void attatch(Tcp::socket socket, Api::Http::request<Api::Http::string_body> request);

private:
    void detach(const std::shared_ptr<Api::WebSocketSession>& session);

    std::shared_ptr<Api::WebSocketManager> websocket_manager_;
    std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager_;
};
} // namespace WebPtt::Core
