#pragma once

#include <expected>
#include <string>
#include <vector>
namespace WebPtt::Audio {
    std::expected<std::vector<float>, std::string> try_read_wav_file(const std::string& filename);
}