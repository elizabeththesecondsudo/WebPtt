#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include <SoundTouch.h>

namespace WebPtt::Audio {
class Tempo {
public:
    explicit Tempo(float tempo);

    void set_tempo(float tempo);

    [[nodiscard]]
    std::vector<float> process(std::span<const float> samples);

private:
    soundtouch::SoundTouch sound_touch_;
};
} // namespace WebPtt::Audio
