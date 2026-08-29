#include "Session.hpp"

#include "Utils/Uuid.hpp"
#include <chrono>
#include <cstddef>
#include <exception>
#include <random>
#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::WebRtc {
namespace {
constexpr std::uint8_t kOpusPayloadType = 111;
constexpr std::uint32_t kOpusSampleRate = 48'000;
constexpr auto kAudioMid = "0";

rtc::SSRC make_ssrc() {
    // An SSRC only needs to be unique within an RTP session. Give every peer
    // connection its own randomly selected source identifier.
    std::mt19937 generator(std::random_device{}());
    return std::uniform_int_distribution<rtc::SSRC>(1, std::numeric_limits<rtc::SSRC>::max())(generator);
}
} // namespace

Session::Session()
    : id_(Utils::generate_uuid())
    , peer_connection_(std::make_shared<rtc::PeerConnection>(rtc::Configuration{})) {
    create_audio_track();
}

void Session::create_audio_track() {
    const auto ssrc = make_ssrc();

    // Chromium assigns MID "0" to its first transceiver. libdatachannel uses
    // the MID to associate the remote m-line with this pre-created track.
    rtc::Description::Audio audio(kAudioMid, rtc::Description::Direction::SendRecv);
    audio.addOpusCodec(kOpusPayloadType);
    audio.addAttribute("ptime:20");
    audio.addAttribute("maxptime:20");
    audio.addSSRC(ssrc, id_, id_, "audio");

    audio_track_ = peer_connection_->addTrack(audio);

    auto rtp_config = std::make_shared<rtc::RtpPacketizationConfig>(ssrc, id_, kOpusPayloadType, kOpusSampleRate);
    auto packetizer = std::make_shared<rtc::OpusRtpPacketizer>(rtp_config);

    // Each call to Track::sendFrame must contain one encoded 20 ms Opus frame.
    // At 48 kHz, consecutive frame timestamps are therefore 960 samples apart.
    packetizer->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtp_config));
    audio_track_->setMediaHandler(packetizer);
}

bool Session::send_audio(rtc::binary opus_frame) {
    if (opus_frame.empty() || !audio_track_->isOpen()) {
        return false;
    }

    const auto frame_number = sent_audio_frames_.fetch_add(1, std::memory_order_relaxed);
    const auto timestamp = std::chrono::duration<double>(static_cast<double>(frame_number) * 0.020);
    audio_track_->sendFrame(std::move(opus_frame), rtc::FrameInfo(timestamp));
    return true;
}

void Session::on_audio(AudioReceiveCallback callback) {
    audio_track_->onMessage(
        [callback = std::move(callback)](rtc::binary packet) {
            if (!callback || packet.size() < sizeof(rtc::RtpHeader) || rtc::IsRtcp(packet)) {
                return;
            }

            spdlog::info("Got {} bytes", packet.size());
            const auto* rtp = reinterpret_cast<const rtc::RtpHeader*>(packet.data());
            if (rtp->version() != 2 || rtp->payloadType() != kOpusPayloadType) {
                return;
            }

            std::size_t payload_offset = sizeof(rtc::RtpHeader) + (rtp->csrcCount() * sizeof(rtc::SSRC));
            if (payload_offset > packet.size()) {
                return;
            }

            if (rtp->extension()) {
                if (packet.size() - payload_offset < sizeof(rtc::RtpExtensionHeader)) {
                    return;
                }
                const auto* extension =
                    reinterpret_cast<const rtc::RtpExtensionHeader*>(packet.data() + payload_offset);
                payload_offset += sizeof(rtc::RtpExtensionHeader) + extension->getSize();
                if (payload_offset > packet.size()) {
                    return;
                }
            }

            std::size_t payload_end = packet.size();
            if (rtp->padding()) {
                const auto padding_size = std::to_integer<std::uint8_t>(packet.back());
                if (padding_size == 0 || padding_size > payload_end - payload_offset) {
                    return;
                }
                payload_end -= padding_size;
            }

            rtc::binary opus_frame(
                packet.begin() + static_cast<std::ptrdiff_t>(payload_offset),
                packet.begin() + static_cast<std::ptrdiff_t>(payload_end));
            callback(std::move(opus_frame), rtp->timestamp());
        },
        nullptr);
}

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
