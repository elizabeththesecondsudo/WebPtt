#pragma once

#include <opus.h>

#include <array>
#include <cstddef>
#include <expected>
#include <span>
#include <string>

namespace WebPtt::Audio {
class OpusTranscoder {
public:
    static constexpr auto kSampleRate = 48'000;
    static constexpr auto kChannels = 1;
    static constexpr auto kSamplesPerFrame = 960;
    static constexpr auto kPacketSize = 160;

    using PcmFrame = std::array<float, kSamplesPerFrame>;
    using OpusPacket = std::array<std::byte, kPacketSize>;

    [[nodiscard]] static std::expected<OpusTranscoder, std::string> make();

    ~OpusTranscoder();

    OpusTranscoder(const OpusTranscoder&) = delete;
    OpusTranscoder& operator=(const OpusTranscoder&) = delete;
    OpusTranscoder(OpusTranscoder&& other) noexcept;
    OpusTranscoder& operator=(OpusTranscoder&& other) noexcept;

    [[nodiscard]] std::expected<OpusPacket, std::string> encode(std::span<const float> pcm);

    [[nodiscard]] std::expected<PcmFrame, std::string> decode(std::span<const std::byte> opus_packet);

private:
    OpusTranscoder(OpusEncoder* encoder, OpusDecoder* decoder) noexcept;

    void reset() noexcept;

    OpusEncoder* encoder_ = nullptr;
    OpusDecoder* decoder_ = nullptr;
};
} // namespace WebPtt::Audio
