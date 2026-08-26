#include "Coordinator.hpp"

#include "Api/WebSocketManager.hpp"
#include "Api/WebSocketSession.hpp"
#include "WebRtc/Messages.hpp"
#include "Utils/Json.hpp"
#include "WebRtc/PeerConnectionManager.hpp"
#include "WebRtc/Session.hpp"
#include <spdlog/spdlog.h>

namespace WebPtt::Core {
Coordinator::Coordinator(
    std::shared_ptr<Api::WebSocketManager> websocket_manager,
    std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager)
    : websocket_manager_(std::move(websocket_manager))
    , peer_connection_manager_(std::move(peer_connection_manager)) {}

void Coordinator::attatch(Tcp::socket socket, Api::Http::request<Api::Http::string_body> request) {
    auto peer_session_res = peer_connection_manager_->create_session();
    if (!peer_session_res) {
        spdlog::error("Failed to create peer connection for WebSocket client: {}", peer_session_res.error());
        return;
    }

    auto peer_session = std::move(peer_session_res.value());

    auto session = std::make_shared<Api::WebSocketSession>(std::move(socket));
    spdlog::debug("Coordinator registering WebSocket session");
    websocket_manager_->add_session(session);

    session->start(
        std::move(request),
        peer_session->id(),
        [](const std::shared_ptr<Api::WebSocketSession>& connected_session) {
            auto message = Utils::serialize_json(SessionCreatedMessage{
                .session_id_ = connected_session->id(),
            });
            if (!message) {
                spdlog::error("Failed to serialize peer session ID: {}", message.error());
                return;
            }

            connected_session->write(std::move(message.value()));
            spdlog::debug("Sent peer session ID {} to WebSocket client", connected_session->id());
        },
        [this](const std::shared_ptr<Api::WebSocketSession>& closed_session) { detach(closed_session); });
}

void Coordinator::detach(const std::shared_ptr<Api::WebSocketSession>& session) {
    spdlog::debug("Coordinator removing WebSocket session");

    if (const auto peer = peer_connection_manager_->find_session(session->id()); peer != nullptr) {
        peer_connection_manager_->remove_session(peer->id());
    }

    websocket_manager_->remove_session(session);
}
} // namespace WebPtt::Core
