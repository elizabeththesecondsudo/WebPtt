#include "Gain.hpp"

#include <algorithm>
#include <cmath>

namespace WebPtt::Audio {
namespace {

constexpr auto kDecibelAmplitudeDivisor = 20.0F;
constexpr auto kDecibelBase = 10.0F;

constexpr auto kMinSample = -1.0F;
constexpr auto kMaxSample = 1.0F;

constexpr float db_to_gain(float decibels) {
    return std::pow(kDecibelBase, decibels / kDecibelAmplitudeDivisor);
}

} // namespace

Gain::Gain(float decibels)
    : gain_factor_(db_to_gain(decibels)) {}

void Gain::process(std::vector<float>& samples) {
    for (auto& sample : samples) {
        const auto amplified_sample = sample * gain_factor_;

        sample = std::clamp(amplified_sample, kMinSample, kMaxSample);
    }
}
} // namespace WebPtt::Audio
