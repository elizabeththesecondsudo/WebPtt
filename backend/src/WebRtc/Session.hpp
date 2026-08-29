#pragma once

#include <rtc/rtc.hpp>

#include <expected>
#include <functional>
#include <memory>
#include <atomic>
#include <string>

namespace WebPtt::WebRtc {
class Session {
public:
    using LocalDescriptionCallback = std::function<void(rtc::Description)>;
    using LocalCandidateCallback = std::function<void(rtc::Candidate)>;
    using AudioReceiveCallback = std::function<void(rtc::binary opus_frame, std::uint32_t rtp_timestamp)>;

    Session();

    [[nodiscard]] const std::string& id() const noexcept;
    void configure(LocalDescriptionCallback on_local_description, LocalCandidateCallback on_local_candidate);
    [[nodiscard]] std::expected<void, std::string> set_remote_description(
        const std::string& sdp,
        const std::string& type);
    [[nodiscard]] std::expected<void, std::string> add_remote_candidate(std::string candidate, std::string mid);

    // Sends one already-encoded 20 ms Opus frame (960 samples per channel).
    [[nodiscard]] bool send_audio(rtc::binary opus_frame);
    void on_audio(AudioReceiveCallback callback);

private:
    void create_audio_track();

    std::string id_;
    std::shared_ptr<rtc::PeerConnection> peer_connection_;
    std::shared_ptr<rtc::Track> audio_track_;
    std::atomic<std::uint64_t> sent_audio_frames_ = 0;
};
} // namespace WebPtt::WebRtc
