#pragma once

#include "Effect.hpp"

#include <cstddef>
#include <vector>

namespace WebPtt::Audio {
class Mixer : public Effect {
public:
    explicit Mixer(std::vector<float> file_content);

    void process(std::vector<float>& samples) override;

private:
    std::vector<float> file_content_;
    size_t current_index_{};
};
} // namespace WebPtt::Audio
