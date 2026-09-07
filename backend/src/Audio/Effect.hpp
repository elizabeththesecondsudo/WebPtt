#pragma once

#include <vector>

namespace WebPtt::Audio {
class Effect {
public:
    Effect() = default;
    virtual ~Effect() = default;
    Effect(const Effect&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect(Effect&&) noexcept = default;
    Effect& operator=(Effect&&) noexcept = default;

    // Processes the existing buffer in place. Stateful effects may resize it
    // or clear it when output is still buffered internally.
    virtual void process(std::vector<float>& samples) = 0;
};
} // namespace WebPtt::Audio
