#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <variant>

namespace WebPtt::Audio {
enum class EffectType : uint8_t { kNone = 0, kGain, kMix, kPitchShift, kTempo };

class Effect;

std::expected<std::unique_ptr<Effect>, std::string> make_effect(
    EffectType type,
    std::variant<float, std::string> value);
} // namespace WebPtt::Audio