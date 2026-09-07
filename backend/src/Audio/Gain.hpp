#pragma once

#include "Effect.hpp"

namespace WebPtt::Audio {
class Gain : public Effect {
public:
    explicit Gain(float decibels);

    void process(std::vector<float>& samples) override;

private:
    float gain_factor_;
};
} // namespace WebPtt::Audio
