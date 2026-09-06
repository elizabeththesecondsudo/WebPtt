#include "Audio/OpusTranscoder.hpp"

#include <opus.h>

#include <cassert>

namespace {
std::expected<void, std::string> configure_encoder(OpusEncoder* encoder) {
    constexpr auto kBitrate = 64'000; // 64 kbps

    auto error = OPUS_OK;
    const auto configure = [&error, encoder](auto... args) {
        if (error == OPUS_OK) {
            error = opus_encoder_ctl(encoder, args...); // NOLINT
        }
    };

    configure(OPUS_SET_BITRATE(kBitrate));
    configure(OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    configure(OPUS_SET_VBR(0));

    if (error != OPUS_OK) {
        return std::unexpected("Failed to configure Opus encoder: " + std::string(opus_strerror(error)));
    }

    return {};
}
} // namespace

namespace WebPtt::Audio {
std::expected<OpusTranscoder, std::string> OpusTranscoder::make() {
    constexpr auto kSampleRate = 48'000;
    constexpr auto kChannels = 1;

    int error = OPUS_OK;
    auto* encoder = opus_encoder_create(kSampleRate, kChannels, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK) {
        return std::unexpected("Failed to create Opus encoder: " + std::string(opus_strerror(error)));
    }

    auto result = configure_encoder(encoder);
    if (!result) {
        opus_encoder_destroy(encoder);
        return std::unexpected(std::move(result.error()));
    }

    auto* decoder = opus_decoder_create(kSampleRate, kChannels, &error);
    if (error != OPUS_OK) {
        opus_encoder_destroy(encoder);
        return std::unexpected("Failed to create Opus decoder: " + std::string(opus_strerror(error)));
    }

    return OpusTranscoder{encoder, decoder};
}

OpusTranscoder::OpusTranscoder(OpusEncoder* encoder, OpusDecoder* decoder) noexcept
    : encoder_(encoder)
    , decoder_(decoder) {}

OpusTranscoder::~OpusTranscoder() {
    reset();
}

OpusTranscoder::OpusTranscoder(OpusTranscoder&& other) noexcept
    : encoder_(other.encoder_)
    , decoder_(other.decoder_) {
    other.encoder_ = nullptr;
    other.decoder_ = nullptr;
}

OpusTranscoder& OpusTranscoder::operator=(OpusTranscoder&& other) noexcept {
    if (this != &other) {
        reset();
        encoder_ = other.encoder_;
        decoder_ = other.decoder_;
        other.encoder_ = nullptr;
        other.decoder_ = nullptr;
    }
    return *this;
}

std::expected<OpusTranscoder::OpusPacket, std::string> OpusTranscoder::encode(const std::span<const float> pcm) {
    if (pcm.size() != kSamplesPerFrame) {
        return std::unexpected("PCM input must contain exactly 960 samples");
    }

    OpusPacket packet{};
    const auto encoded_size = opus_encode_float(
        encoder_,
        pcm.data(),
        static_cast<int>(kSamplesPerFrame),
        packet.data(),
        static_cast<opus_int32>(packet.size()));

    if (encoded_size < 0) {
        return std::unexpected("Failed to encode Opus frame: " + std::string(opus_strerror(encoded_size)));
    }

    if (encoded_size != static_cast<int>(kPacketSize)) {
        return std::unexpected("Opus encoder did not produce the required 160-byte packet");
    }

    return packet;
}

std::expected<OpusTranscoder::PcmFrame, std::string> OpusTranscoder::decode(
    const std::span<const std::uint8_t> opus_packet) {
    if (opus_packet.size() != kPacketSize) {
        return std::unexpected("Opus packet must contain exactly 160 bytes");
    }

    PcmFrame pcm{};
    const auto decoded_samples = opus_decode_float(
        decoder_,
        opus_packet.data(),
        static_cast<opus_int32>(opus_packet.size()),
        pcm.data(),
        static_cast<int>(kSamplesPerFrame),
        0);

    if (decoded_samples < 0) {
        return std::unexpected("Failed to decode Opus packet: " + std::string(opus_strerror(decoded_samples)));
    }

    if (decoded_samples != static_cast<int>(kSamplesPerFrame)) {
        return std::unexpected("Opus packet did not decode to the required 960 samples");
    }

    return pcm;
}

void OpusTranscoder::reset() noexcept {
    if (encoder_ != nullptr) {
        opus_encoder_destroy(encoder_);
        encoder_ = nullptr;
    }

    if (decoder_ != nullptr) {
        opus_decoder_destroy(decoder_);
        decoder_ = nullptr;
    }
}
} // namespace WebPtt::Audio
