#include "Coordinator.hpp"

#include "Api/WebSocketManager.hpp"
#include "Api/WebSocketSession.hpp"
#include <spdlog/spdlog.h>

namespace WebPtt::Core {
Coordinator::Coordinator(std::shared_ptr<Api::WebSocketManager> websocket_manager)
    : websocket_manager_(std::move(websocket_manager)) {}

void Coordinator::attatch(Tcp::socket socket, Api::Http::request<Api::Http::string_body> request) {
    auto session = std::make_shared<Api::WebSocketSession>(std::move(socket));
    spdlog::debug("Coordinator registering WebSocket session");
    websocket_manager_->add_session(session);

    session->start(std::move(request), [this](const std::shared_ptr<Api::WebSocketSession>& closed_session) {
        detach(closed_session);
    });
}

void Coordinator::detach(const std::shared_ptr<Api::WebSocketSession>& session) {
    spdlog::debug("Coordinator removing WebSocket session");
    websocket_manager_->remove_session(session);
}
} // namespace WebPtt::Core
