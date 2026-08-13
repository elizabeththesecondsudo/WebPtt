#include "Gain.hpp"
#include <algorithm>
#include <cmath>

namespace MediaPlane::Dsp {
namespace {
constexpr int16_t kInt16Max = std::numeric_limits<int16_t>::max();
constexpr int16_t kInt16Min = std::numeric_limits<int16_t>::min();

constexpr float decibels_to_gain(float decibels) {
    constexpr float kDecibelBase = 10.0F;
    constexpr float kAmplitudeDecibelScale = 20.0F;
    return std::pow(kDecibelBase, decibels / kAmplitudeDecibelScale);
}
} // namespace

Gain::Gain(float decibels)
    : gain_factor_(decibels_to_gain(decibels)) {}

void Gain::apply(std::span<int16_t> pcm_samples) {
    for (auto& sample : pcm_samples) {
        const float amplified = static_cast<float>(sample) * gain_factor_;

        const float clamped = std::clamp(amplified, static_cast<float>(kInt16Min), static_cast<float>(kInt16Max));

        sample = static_cast<int16_t>(clamped);
    }
}
} // namespace MediaPlane::Dsp
