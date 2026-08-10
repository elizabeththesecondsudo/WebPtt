#pragma once

#include <cstdint>
#include <span>

namespace WebPtt::Dsp {
class Effect {
public:
    Effect() = default;
    Effect(const Effect&) = default;
    Effect(Effect&&) noexcept = default;
    Effect& operator=(const Effect&) = default;
    Effect& operator=(Effect&&) noexcept = default;
    virtual ~Effect() = default;

    virtual void apply(std::span<int16_t> pcm_samples) = 0;
};
} // namespace WebPtt::Dsp