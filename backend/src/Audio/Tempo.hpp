#pragma once

#include "Effect.hpp"

#include <cstddef>
#include <vector>

#include <SoundTouch.h>

namespace WebPtt::Audio {
class Tempo : public Effect {
public:
    explicit Tempo(float tempo);

    void set_tempo(float tempo);

    void process(std::vector<float>& samples) override;

private:
    soundtouch::SoundTouch sound_touch_;
};
} // namespace WebPtt::Audio
