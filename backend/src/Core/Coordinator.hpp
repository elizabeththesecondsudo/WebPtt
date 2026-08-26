#pragma once

#include "Api/Types.hpp"
#include "Types.hpp"

#include <memory>

namespace WebPtt::Api {
class WebSocketManager;
class WebSocketSession;
} // namespace WebPtt::Api

namespace WebPtt::WebRtc {
class PeerConnectionManager;
class Session;
} // namespace WebPtt::WebRtc

namespace WebPtt::Core {
class Coordinator {
public:
    Coordinator(
        std::shared_ptr<Api::WebSocketManager> websocket_manager,
        std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager);

    void attatch(Tcp::socket socket, Api::Http::request<Api::Http::string_body> request);

private:
    static void configure_peer_connection(
        const std::shared_ptr<WebRtc::Session>& peer_session,
        const std::shared_ptr<Api::WebSocketSession>& websocket_session);
    void start_websocket_session(
        const std::shared_ptr<WebRtc::Session>& peer_session,
        const std::shared_ptr<Api::WebSocketSession>& websocket_session,
        Api::Http::request<Api::Http::string_body> request);
    void detach(const std::shared_ptr<Api::WebSocketSession>& session);

    std::shared_ptr<Api::WebSocketManager> websocket_manager_;
    std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager_;
};
} // namespace WebPtt::Core
