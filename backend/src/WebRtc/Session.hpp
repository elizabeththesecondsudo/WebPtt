#pragma once

#include <rtc/rtc.hpp>

#include <memory>
#include <string>

namespace WebPtt::WebRtc {
class Session {
public:
    Session();

    [[nodiscard]] const std::string& id() const noexcept;
    [[nodiscard]] const std::shared_ptr<rtc::PeerConnection>& peer_connection() const noexcept;

private:
    std::string id_;
    std::shared_ptr<rtc::PeerConnection> peer_connection_;
};
} // namespace WebPtt::WebRtc
