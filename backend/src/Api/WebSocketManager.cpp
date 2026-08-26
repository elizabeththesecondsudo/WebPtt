#include "WebSocketManager.hpp"
#include "WebSocketSession.hpp"

#include <spdlog/spdlog.h>

namespace WebPtt::Api {
void WebSocketManager::add_session(std::shared_ptr<WebSocketSession> session) {
    sessions_.push_back(std::move(session));
    spdlog::info("Registered WebSocket session; active sessions: {}", sessions_.size());
}

void WebSocketManager::remove_session(const std::shared_ptr<WebSocketSession>& session) {
    const auto removed = std::erase(sessions_, session);
    if (removed == 0) {
        spdlog::debug("WebSocket session was already removed");
        return;
    }

    spdlog::info("Removed WebSocket session; active sessions: {}", sessions_.size());
}
} // namespace WebPtt::Api
