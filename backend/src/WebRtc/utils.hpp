#pragma once

#include <rtc/peerconnection.hpp>
#include <string_view>

namespace WebPtt::WebRtc {

constexpr std::string_view state_name(rtc::PeerConnection::State state) {
    using State = rtc::PeerConnection::State;
    switch (state) {
    case State::New: return "new";
    case State::Connecting: return "connecting";
    case State::Connected: return "connected";
    case State::Disconnected: return "disconnected";
    case State::Failed: return "failed";
    case State::Closed: return "closed";
    }
    return "unknown";
}

constexpr std::string_view state_name(rtc::PeerConnection::IceState state) {
    using State = rtc::PeerConnection::IceState;
    switch (state) {
    case State::New: return "new";
    case State::Checking: return "checking";
    case State::Connected: return "connected";
    case State::Completed: return "completed";
    case State::Failed: return "failed";
    case State::Disconnected: return "disconnected";
    case State::Closed: return "closed";
    }
    return "unknown";
}

constexpr std::string_view state_name(rtc::PeerConnection::GatheringState state) {
    using State = rtc::PeerConnection::GatheringState;
    switch (state) {
    case State::New: return "new";
    case State::InProgress: return "in-progress";
    case State::Complete: return "complete";
    }
    return "unknown";
}

constexpr std::string_view state_name(rtc::PeerConnection::SignalingState state) {
    using State = rtc::PeerConnection::SignalingState;
    switch (state) {
    case State::Stable: return "stable";
    case State::HaveLocalOffer: return "have-local-offer";
    case State::HaveRemoteOffer: return "have-remote-offer";
    case State::HaveLocalPranswer: return "have-local-pranswer";
    case State::HaveRemotePranswer: return "have-remote-pranswer";
    }
    return "unknown";
}

} // namespace WebPtt::WebRtc
