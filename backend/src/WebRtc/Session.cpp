#include "Session.hpp"

#include "Utils/Uuid.hpp"

namespace WebPtt::WebRtc {
Session::Session()
    : id_(Utils::generate_uuid())
    , peer_connection_(std::make_shared<rtc::PeerConnection>(rtc::Configuration{})) {}

const std::string& Session::id() const noexcept {
    return id_;
}

const std::shared_ptr<rtc::PeerConnection>& Session::peer_connection() const noexcept {
    return peer_connection_;
}
} // namespace WebPtt::WebRtc
