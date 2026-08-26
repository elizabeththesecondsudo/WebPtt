#pragma once

#include <glaze/json.hpp>
#include <string>

namespace WebPtt::Core {
struct SessionCreatedMessage {
    std::string type_{"session_created"};
    std::string session_id_;
};
} // namespace WebPtt::Core
