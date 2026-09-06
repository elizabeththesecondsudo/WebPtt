#include "Session.hpp"
#include "rtc/rtp.hpp"
#include "utils.hpp"

#include <RtpCpp/RtpPacket.hpp>

#include "Utils/Uuid.hpp"
#include <chrono>
#include <cstddef>
#include <exception>
#include <memory>
#include <random>
#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::WebRtc {
namespace {
constexpr std::uint8_t kOpusPayloadType = 111;
constexpr std::uint32_t kOpusSampleRate = 48'000;
constexpr auto kAudioMid = "0";
constexpr auto kOpusProfile =
    "minptime=20;maxaveragebitrate=64000;stereo=0;sprop-stereo=0;useinbandfec=1;usedtx=0;cbr=1";

rtc::SSRC make_ssrc() {
    // An SSRC only needs to be unique within an RTP session. Give every peer
    // connection its own randomly selected source identifier.
    std::mt19937 generator(std::random_device{}());
    return std::uniform_int_distribution<rtc::SSRC>(1, std::numeric_limits<rtc::SSRC>::max())(generator);
}
} // namespace

Session::Session(std::optional<Audio::OpusTranscoder> opus_transcoder)
    : id_(Utils::generate_uuid())
    , peer_connection_(std::make_shared<rtc::PeerConnection>(rtc::Configuration{}))
    , opus_transcoder_(std::move(opus_transcoder)) {
    create_audio_track();
}

void Session::create_audio_track() {
    const auto ssrc = make_ssrc();

    // Chromium assigns MID "0" to its first transceiver. libdatachannel uses
    // the MID to associate the remote m-line with this pre-created track.
    rtc::Description::Audio audio(kAudioMid, rtc::Description::Direction::SendRecv);
    // Ask the browser encoder for mono, constant-bitrate Opus without DTX so
    // silent frames retain the same pacing and approximately the same size.
    audio.addOpusCodec(kOpusPayloadType, kOpusProfile);
    audio.addAttribute("ptime:20");
    audio.addAttribute("maxptime:20");
    audio.addSSRC(ssrc, id_, id_, "audio");

    audio_track_ = peer_connection_->addTrack(audio);
    spdlog::info(
        "Peer {} created audio track: mid={}, local_ssrc={}, payload_type={}",
        id_,
        kAudioMid,
        ssrc,
        kOpusPayloadType);

    const auto session_id = id_;
    audio_track_->onOpen([session_id]() { spdlog::info("Peer {} audio track opened", session_id); });
    audio_track_->onClosed([session_id]() { spdlog::info("Peer {} audio track closed", session_id); });

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
    auto received_packets = std::make_shared<std::atomic<std::uint64_t>>(0);
    audio_track_->onMessage(
        [this, callback = std::move(callback), received_packets](rtc::binary packet) {
            if (rtc::IsRtcp(packet)) {
                return;
            }

            RtpCpp::RtpPacketView rtp;

            std::span<std::uint8_t> packet_view(reinterpret_cast<uint8_t*>(packet.data()), packet.size()); // NOLINT

            const auto result = rtp.parse(packet_view);
            if (result != RtpCpp::Result::kSuccess) {
                spdlog::warn("Peer {} dropped invalid RTP packet: {}", id_, RtpCpp::make_error_code(result).message());
                return;
            }

            const auto& header = rtp.get_header();
            if (header.payload_type_ != kOpusPayloadType) {
                spdlog::warn(
                    "Peer {} dropped RTP: payload_type={}, expected_payload_type={}",
                    id_,
                    header.payload_type_,
                    kOpusPayloadType);
                return;
            }

            const auto payload = rtp.payload();
            const auto* payload_begin = reinterpret_cast<const std::byte*>(payload.data());

            if (is_buffering_command_ && opus_transcoder_) {
                const auto decoded = opus_transcoder_->decode(payload);
                if (decoded) {
                    command_buffer_.insert(command_buffer_.end(), decoded->begin(), decoded->end());
                    spdlog::debug(
                        "Peer {} decoded Opus audio: {} samples, total buffered command samples={}",
                        id_,
                        decoded->size(),
                        command_buffer_.size());
                }
                else {
                    spdlog::warn("Peer {} failed to decode Opus audio: {}", id_, decoded.error());
                }
            }
            else {
                rtc::binary opus_frame(payload_begin, payload_begin + payload.size());
                const auto count = received_packets->fetch_add(1, std::memory_order_relaxed) + 1;
                callback(std::move(opus_frame), header.timestamp_);
            }
        },
        nullptr);
}

bool Session::is_buffering_command() const {
    return is_buffering_command_;
}

void Session::set_buffering_command(bool buffering) {
    is_buffering_command_ = buffering;
}

std::vector<float> Session::take_command_buffer() {
    std::vector<float> buffer;
    buffer.swap(command_buffer_);
    return buffer;
}

const std::string& Session::id() const noexcept {
    return id_;
}

void Session::configure(LocalDescriptionCallback on_local_description, LocalCandidateCallback on_local_candidate) {
    peer_connection_->onLocalDescription(std::move(on_local_description));
    peer_connection_->onLocalCandidate(std::move(on_local_candidate));

    const auto session_id = id_;

    peer_connection_->onStateChange([session_id](rtc::PeerConnection::State state) {
        spdlog::info(
            "Peer {} connection state changed to {} ({})",
            session_id,
            state_name(state),
            static_cast<int>(state));
    });

    peer_connection_->onIceStateChange([session_id](rtc::PeerConnection::IceState state) {
        spdlog::info("Peer {} ICE state changed to {} ({})", session_id, state_name(state), static_cast<int>(state));
    });

    peer_connection_->onGatheringStateChange([session_id](rtc::PeerConnection::GatheringState state) {
        spdlog::debug(
            "Peer {} ICE gathering state changed to {} ({})",
            session_id,
            state_name(state),
            static_cast<int>(state));
    });

    peer_connection_->onSignalingStateChange([session_id](rtc::PeerConnection::SignalingState state) {
        spdlog::debug(
            "Peer {} signaling state changed to {} ({})",
            session_id,
            state_name(state),
            static_cast<int>(state));
    });
}

std::expected<void, std::string> Session::set_remote_description(const std::string& sdp, const std::string& type) {
    try {
        spdlog::info("Peer {} applying remote {} SDP ({} bytes):\n{}", id_, type, sdp.size(), sdp);
        peer_connection_->setRemoteDescription(rtc::Description(sdp, type));
        spdlog::info("Peer {} applied remote {} SDP successfully", id_, type);
        return {};
    } catch (const std::exception& error) {
        spdlog::error("Peer {} rejected remote {} SDP: {}", id_, type, error.what());
        return std::unexpected(error.what());
    }
}

std::expected<void, std::string> Session::add_remote_candidate(std::string candidate, std::string mid) {
    try {
        spdlog::info("Peer {} adding remote ICE candidate for mid {}: {}", id_, mid, candidate);
        peer_connection_->addRemoteCandidate(rtc::Candidate(std::move(candidate), std::move(mid)));
        return {};
    } catch (const std::exception& error) {
        return std::unexpected(error.what());
    }
}
} // namespace WebPtt::WebRtc
