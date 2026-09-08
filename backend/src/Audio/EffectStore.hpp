#pragma once

#include "Effect.hpp"
#include "EffectFactory.hpp"

#include <expected>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace WebPtt::Audio {
class EffectStore {
public:
    // Appends an effect on success; leaves the pipeline unchanged on failure.
    [[nodiscard]] std::expected<void, std::string> add_effect(
        EffectType type,
        std::variant<float, std::string> value);

    // Applies effects in insertion order, preserving their state across packets.
    // Effects may resize or clear the PCM buffer.
    void process(std::vector<float>& samples);

private:
    std::vector<std::unique_ptr<Effect>> effects_;
};
} // namespace WebPtt::Audio
