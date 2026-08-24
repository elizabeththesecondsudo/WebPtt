#pragma once

#include <string>

namespace WebPtt::Api {
struct SessionCreatedMessage {
    std::string type_{"session_created"};
    std::string session_id_;
};
} // namespace WebPtt::Api
