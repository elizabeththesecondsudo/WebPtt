#include "Session.hpp"

#include "Utils/Uuid.hpp"
#include <exception>
#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::WebRtc {
Session::Session()
    : id_(Utils::generate_uuid())
    , peer_connection_(std::make_shared<rtc::PeerConnection>(rtc::Configuration{})) {}

const std::string& Session::id() const noexcept {
    return id_;
}

void Session::configure(LocalDescriptionCallback on_local_description, LocalCandidateCallback on_local_candidate) {
    peer_connection_->onLocalDescription(std::move(on_local_description));
    peer_connection_->onLocalCandidate(std::move(on_local_candidate));

    const auto session_id = id_;

    peer_connection_->onStateChange([session_id](rtc::PeerConnection::State state) {
        spdlog::info("Peer {} connection state changed to {}", session_id, static_cast<int>(state));
    });

    peer_connection_->onIceStateChange([session_id](rtc::PeerConnection::IceState state) {
        spdlog::info("Peer {} ICE state changed to {}", session_id, static_cast<int>(state));
    });

    peer_connection_->onGatheringStateChange([session_id](rtc::PeerConnection::GatheringState state) {
        spdlog::debug("Peer {} ICE gathering state changed to {}", session_id, static_cast<int>(state));
    });

    peer_connection_->onSignalingStateChange([session_id](rtc::PeerConnection::SignalingState state) {
        spdlog::debug("Peer {} signaling state changed to {}", session_id, static_cast<int>(state));
    });
}

std::expected<void, std::string> Session::set_remote_description(const std::string& sdp, const std::string& type) {
    try {
        peer_connection_->setRemoteDescription(rtc::Description(sdp, type));
        return {};
    } catch (const std::exception& error) {
        return std::unexpected(error.what());
    }
}

std::expected<void, std::string> Session::add_remote_candidate(std::string candidate, std::string mid) {
    try {
        peer_connection_->addRemoteCandidate(rtc::Candidate(std::move(candidate), std::move(mid)));
        return {};
    } catch (const std::exception& error) {
        return std::unexpected(error.what());
    }
}
} // namespace WebPtt::WebRtc
