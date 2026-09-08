#include "Wav.hpp"
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

namespace WebPtt::Audio {
std::expected<std::vector<float>, std::string> try_read_wav_file(const std::string& filename) {
    constexpr drwav_uint32 kSampleRate = 48'000;
    constexpr drwav_uint32 kChannels = 1;

    drwav wav{};

    if (drwav_init_file(&wav, filename.c_str(), nullptr) == 0U) {
        return std::unexpected("Failed to open WAV file" + filename);
    }

    if (wav.sampleRate != kSampleRate) {
        drwav_uninit(&wav);
        return std::unexpected("WAV sample rate must be 48000 Hz");
    }

    if (wav.channels != kChannels) {
        drwav_uninit(&wav);
        return std::unexpected("WAV must be mono");
    }

    if (wav.translatedFormatTag != DR_WAVE_FORMAT_PCM) {
        drwav_uninit(&wav);
        return std::unexpected("WAV must contain PCM audio");
    }

    const auto frame_count = wav.totalPCMFrameCount;

    std::vector<float> samples(frame_count);

    const auto frames_read = drwav_read_pcm_frames_f32(&wav, frame_count, samples.data());

    drwav_uninit(&wav);

    if (frames_read != frame_count) {
        return std::unexpected("Failed to read all WAV samples");
    }

    return samples;
}
} // namespace WebPtt::Audio