#pragma once

#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace WebPtt::WebRtc {
class Session;

class PeerConnectionManager {
public:
    [[nodiscard]] std::expected<std::shared_ptr<Session>, std::string> create_session();
    [[nodiscard]] std::shared_ptr<Session> find_session(std::string_view peer_id) const;
    void remove_session(std::string_view peer_id);

private:
    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
};
} // namespace WebPtt::WebRtc
