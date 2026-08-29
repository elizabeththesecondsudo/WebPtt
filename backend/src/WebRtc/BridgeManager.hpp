#pragma once

#include <expected>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace WebPtt::WebRtc {
class Bridge;
class PeerConnectionManager;

enum class BridgeErrorCode { invalid_request, session_not_found, session_busy };

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

private:
    std::shared_ptr<PeerConnectionManager> peer_connection_manager_;
    std::unordered_map<std::string, std::shared_ptr<Bridge>> bridges_by_session_;
    std::mutex mutex_;
};
} // namespace WebPtt::WebRtc
