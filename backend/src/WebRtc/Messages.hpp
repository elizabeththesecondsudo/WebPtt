#pragma once

#include <glaze/json.hpp>
#include <string>

namespace WebPtt::Core {
struct SessionCreatedMessage {
    std::string type_{"session_created"};
    std::string session_id_;
};

struct SessionDescriptionMessage {
    std::string type_;
    std::string sdp_;
};

struct IceCandidateMessage {
    std::string type_{"candidate"};
    std::string candidate_;
    std::string sdp_mid_;
};
} // namespace WebPtt::Core
