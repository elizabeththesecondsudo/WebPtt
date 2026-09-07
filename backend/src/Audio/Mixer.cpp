#include "Mixer.hpp"

#include <cmath>
#include <utility>

namespace WebPtt::Audio {
Mixer::Mixer(std::vector<float> file_content)
    : file_content_(std::move(file_content)) {}

void Mixer::process(std::span<float> samples) {
    if (file_content_.empty()) {
        return;
    }

    for (auto& sample : samples) {
        const auto mixed_sample = sample + file_content_[current_index_];

        sample = std::tanh(mixed_sample);

        ++current_index_;

        if (current_index_ == file_content_.size()) {
            current_index_ = 0;
        }
    }
}
} // namespace WebPtt::Audio