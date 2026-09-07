#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace WebPtt::Audio {
class Mixer {
public:
    explicit Mixer(std::vector<float> file_content);

    void process(std::span<float> samples);

private:
    std::vector<float> file_content_;
    size_t current_index_{};
};
} // namespace WebPtt::Audio