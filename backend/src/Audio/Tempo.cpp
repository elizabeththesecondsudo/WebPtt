#include "Tempo.hpp"

#include <algorithm>
#include <vector>

namespace WebPtt::Audio {
namespace {
constexpr auto kSampleRate = 48'000;
constexpr auto kChannels = 1;

constexpr float clamp_tempo(float tempo) {
    constexpr auto kMinTempo = 0.0F;
    constexpr auto kMaxTempo = 4.0F;
    return std::clamp(tempo, kMinTempo, kMaxTempo);
}

} // namespace

Tempo::Tempo(float tempo) {
    sound_touch_.setSampleRate(static_cast<unsigned int>(kSampleRate));

    sound_touch_.setChannels(static_cast<unsigned int>(kSampleRate));

    sound_touch_.setTempo(clamp_tempo(tempo));
}

std::vector<float> Tempo::process(std::span<const float> samples) {
    constexpr std::size_t kReceiveBufferFrames = 4096;
    if (samples.empty()) {
        return {};
    }

    const std::size_t input_frames = samples.size() / kChannels;

    sound_touch_.putSamples(samples.data(), static_cast<unsigned int>(input_frames));

    std::vector<float> output;

    std::vector<float> receive_buffer(kReceiveBufferFrames * kChannels);

    while (true) {
        const unsigned int received_frames =
            sound_touch_.receiveSamples(receive_buffer.data(), static_cast<unsigned int>(kReceiveBufferFrames));

        if (received_frames == 0) {
            break;
        }

        const std::size_t received_samples = static_cast<std::size_t>(received_frames) * kChannels;

        output.insert(
            output.end(),
            receive_buffer.begin(),
            receive_buffer.begin() + static_cast<long>(received_samples));
    }

    return output;
}
} // namespace WebPtt::Audio