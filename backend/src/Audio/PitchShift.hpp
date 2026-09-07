#pragma once

#include "Effect.hpp"

#include <vector>

#include <SoundTouch.h>

namespace WebPtt::Audio {

class PitchShift : public Effect {
public:
    explicit PitchShift(float semitones);

    void process(std::vector<float>& samples) override;

private:
    soundtouch::SoundTouch sound_touch_;
};

} // namespace WebPtt::Audio
