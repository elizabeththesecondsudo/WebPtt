#pragma once

#include "Effect.hpp"

namespace WebPtt::Dsp {
class Gain final : public Effect {
public:
    explicit Gain(float decibels = 0);
    void apply(std::span<int16_t> pcm_samples) override;

private:
    float gain_factor_;
};
} // namespace WebPtt::Dsp
