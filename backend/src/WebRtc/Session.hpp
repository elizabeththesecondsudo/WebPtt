#pragma once

#include <rtc/rtc.hpp>

#include <expected>
#include <functional>
#include <memory>
#include <string>

namespace WebPtt::WebRtc {
class Session {
public:
    using LocalDescriptionCallback = std::function<void(rtc::Description)>;
    using LocalCandidateCallback = std::function<void(rtc::Candidate)>;

    Session();

    [[nodiscard]] const std::string& id() const noexcept;
    void configure(LocalDescriptionCallback on_local_description, LocalCandidateCallback on_local_candidate);
    [[nodiscard]] std::expected<void, std::string> set_remote_description(
        const std::string& sdp,
        const std::string& type);
    [[nodiscard]] std::expected<void, std::string> add_remote_candidate(std::string candidate, std::string mid);

private:
    std::string id_;
    std::shared_ptr<rtc::PeerConnection> peer_connection_;
};
} // namespace WebPtt::WebRtc
