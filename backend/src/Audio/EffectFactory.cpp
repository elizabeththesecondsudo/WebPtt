#include "EffectFactory.hpp"
#include <format>
#include <memory>
#include <string_view>
#include <variant>
#include "Audio/Wav.hpp"
#include "Gain.hpp"
#include "Mixer.hpp"
#include "Tempo.hpp"
#include "PitchShift.hpp"

namespace WebPtt::Audio {
namespace {
template <typename T>
std::expected<std::unique_ptr<Effect>, std::string> make_float_effect(
    const std::variant<float, std::string>& value,
    std::string_view name) {
    if (const auto* number = std::get_if<float>(&value)) {
        return std::make_unique<T>(*number);
    }
    return std::unexpected(std::format("Got unexpected value type for {} effect - expected float got string", name));
}
} // namespace

std::expected<std::unique_ptr<Effect>, std::string> make_effect(
    EffectType type,
    std::variant<float, std::string> value) {
    switch (type) {
    case EffectType::kGain: return make_float_effect<Gain>(value, "gain");
    case EffectType::kMix: {
        const auto* filename = std::get_if<std::string>(&value);
        if (filename == nullptr) {
            return std::unexpected("Got unexpected value type for mix effect - expected string got float");
        }
        auto samples = try_read_wav_file(*filename);
        if (!samples) {
            return std::unexpected(samples.error());
        }
        return std::make_unique<Mixer>(std::move(*samples));
    }
    case EffectType::kPitchShift: return make_float_effect<PitchShift>(value, "pitch shift");
    case EffectType::kTempo: return make_float_effect<Tempo>(value, "tempo");
    default: break;
    }

    return std::unexpected(std::format("Unknown effect type: {}", static_cast<int>(type)));
}
} // namespace WebPtt::Audio
