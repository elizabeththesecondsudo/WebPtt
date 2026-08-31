#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace WebPtt::WebRtc {
class Bridge;
class PeerConnectionManager;

enum class BridgeErrorCode : uint8_t { kInvalidRequest, kSessionNotFound, kSessionBusy };

struct BridgeError {
    BridgeErrorCode code_;
    std::string message_;
};

class BridgeManager {
public:
    explicit BridgeManager(std::shared_ptr<PeerConnectionManager> peer_connection_manager);

    [[nodiscard]] std::expected<std::shared_ptr<Bridge>, BridgeError> create_bridge(
        std::string_view session_id,
        std::string_view target_session_id);
    [[nodiscard]] bool remove_bridge(std::string_view bridge_id);

private:
    std::shared_ptr<PeerConnectionManager> peer_connection_manager_;
    std::unordered_map<std::string, std::shared_ptr<Bridge>> bridges_by_session_;
};
} // namespace WebPtt::WebRtc
