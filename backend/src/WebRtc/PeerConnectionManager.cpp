#include "PeerConnectionManager.hpp"

#include "Audio/OpusTranscoder.hpp"
#include "Session.hpp"

#include <optional>
#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::WebRtc {
std::expected<std::shared_ptr<Session>, std::string> PeerConnectionManager::create_session() {
    std::optional<Audio::OpusTranscoder> opus_transcoder;
    auto opus_transcoder_result = Audio::OpusTranscoder::make();
    if (opus_transcoder_result) {
        opus_transcoder.emplace(std::move(*opus_transcoder_result));
    }
    else {
        spdlog::warn(
            "Could not create Opus transcoder; peer will operate in relay-only mode: {}",
            opus_transcoder_result.error());
    }

    auto session = std::make_shared<Session>(std::move(opus_transcoder));
    const auto [itr, inserted] = sessions_.emplace(session->id(), session);
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
