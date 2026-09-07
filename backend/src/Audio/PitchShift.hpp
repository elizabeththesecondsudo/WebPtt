#pragma once

#include <span>
#include <vector>

#include <SoundTouch.h>

namespace WebPtt::Audio {

class PitchShift {
public:
    explicit PitchShift(float semitones);

    [[nodiscard]]
    std::vector<float> process(std::span<const float> samples);

private:
    soundtouch::SoundTouch sound_touch_;
};

} // namespace WebPtt::Audio