#include "BridgeManager.hpp"

#include "Bridge.hpp"
#include "PeerConnectionManager.hpp"

#include <algorithm>

namespace WebPtt::WebRtc {
BridgeManager::BridgeManager(std::shared_ptr<PeerConnectionManager> peer_connection_manager)
    : peer_connection_manager_(std::move(peer_connection_manager)) {}

std::expected<std::shared_ptr<Bridge>, BridgeError> BridgeManager::create_bridge(
    std::string_view session_id,
    std::string_view target_session_id) {
    if (session_id.empty() || target_session_id.empty() || session_id == target_session_id) {
        return std::unexpected(
            BridgeError{
                .code_ = BridgeErrorCode::kInvalidRequest,
                .message_ = "session_id and target_session_id must identify two different sessions",
            });
    }

    const auto session = peer_connection_manager_->find_session(session_id);
    const auto target = peer_connection_manager_->find_session(target_session_id);
    if (!session || !target) {
        return std::unexpected(
            BridgeError{
                .code_ = BridgeErrorCode::kSessionNotFound,
                .message_ = !session ? "session_id was not found" : "target_session_id was not found",
            });
    }

    for (const auto uuid : {session_id, target_session_id}) {
        auto existing = bridges_by_session_.find(std::string(uuid));
        if (existing != bridges_by_session_.end() && existing->second->active()) {
            return std::unexpected(
                BridgeError{
                    .code_ = BridgeErrorCode::kSessionBusy,
                    .message_ = "one of the sessions is already connected to a bridge",
                });
        }
        if (existing != bridges_by_session_.end()) {
            bridges_by_session_.erase(existing);
        }
    }

    auto bridge = std::make_shared<Bridge>(session, target);
    bridge->connect();
    bridges_by_session_.emplace(std::string(session_id), bridge);
    bridges_by_session_.emplace(std::string(target_session_id), bridge);
    return bridge;
}

bool BridgeManager::remove_bridge(std::string_view bridge_id) {
    const auto matching = std::ranges::find_if(
        bridges_by_session_,
        [bridge_id](const auto& entry) { return entry.second->id() == bridge_id; });
    if (matching == bridges_by_session_.end()) {
        return false;
    }

    const auto bridge = matching->second;
    bridge->disconnect();
    std::erase_if(bridges_by_session_, [&bridge](const auto& entry) { return entry.second == bridge; });
    return true;
}
} // namespace WebPtt::WebRtc
