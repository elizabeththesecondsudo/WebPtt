#pragma once

#include <span>

namespace WebPtt::Audio {
class Gain {
public:
    explicit Gain(float decibels);

    void process(std::span<float> samples) const;

private:
    float gain_factor_;
};
} // namespace WebPtt::Audio