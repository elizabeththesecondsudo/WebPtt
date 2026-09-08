#include "EffectStore.hpp"

#include <utility>

namespace WebPtt::Audio {
std::expected<void, std::string> EffectStore::add_effect(
    EffectType type,
    std::variant<float, std::string> value) {
    auto effect = make_effect(type, std::move(value));
    if (!effect) {
        return std::unexpected(std::move(effect.error()));
    }
    effects_.push_back(std::move(*effect));
    return {};
}

void EffectStore::process(std::vector<float>& samples) {
    for (const auto& effect : effects_) {
        effect->process(samples);
    }
}
} // namespace WebPtt::Audio
