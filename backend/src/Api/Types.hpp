#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <string>

namespace WebPtt::Api {
namespace Http = boost::beast::http;
namespace WebSocket = boost::beast::websocket;

struct CreateSessionRequest {
    std::string session_id_;
    std::string target_session_id_;
};

struct CreateSessionResponse {
    std::string bridge_id_;
    std::string session_id_;
    std::string target_session_id_;
};

struct DeleteSessionRequest {
    std::string bridge_id_;
};

struct SessionStatusRequest {
    std::string session_id_;
};

struct SessionStatusResponse {
    bool active_{};
    std::string bridge_id_;
    std::string source_session_id_;
    std::string target_session_id_;
};

struct ErrorResponse {
    std::string error_;
};
} // namespace WebPtt::Api
