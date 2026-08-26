#include "PeerConnectionManager.hpp"

#include "Session.hpp"

#include <spdlog/spdlog.h>

namespace WebPtt::WebRtc {
std::expected<std::shared_ptr<Session>, std::string> PeerConnectionManager::create_session() {
    auto session = std::make_shared<Session>();
    const auto [_, inserted] = sessions_.emplace(session->id(), session);
    if (!inserted) {
        return std::unexpected("Generated duplicate peer ID: {}" + session->id());
    }

    spdlog::info("Created peer {}; active peers: {}", session->id(), sessions_.size());
    return session;
}

std::shared_ptr<Session> PeerConnectionManager::find_session(std::string_view peer_id) const {
    const auto session = sessions_.find(std::string(peer_id));
    return session == sessions_.end() ? nullptr : session->second;
}

void PeerConnectionManager::remove_session(std::string_view peer_id) {
    const auto removed = sessions_.erase(std::string(peer_id));
    if (removed == 0) {
        spdlog::debug("Peer {} was already removed", peer_id);
        return;
    }

    spdlog::info("Removed peer {}; active peers: {}", peer_id, sessions_.size());
}
} // namespace WebPtt::WebRtc
