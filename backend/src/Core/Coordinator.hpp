#pragma once

#include "Api/Types.hpp"
#include "Types.hpp"

#include <memory>

namespace WebPtt::Api {
class WebSocketManager;
class WebSocketSession;
} // namespace WebPtt::Api

namespace WebPtt::Core {
class Coordinator {
public:
    explicit Coordinator(std::shared_ptr<Api::WebSocketManager> websocket_manager);

    void attatch(Tcp::socket socket, Api::Http::request<Api::Http::string_body> request);

private:
    void detach(const std::shared_ptr<Api::WebSocketSession>& session);

    std::shared_ptr<Api::WebSocketManager> websocket_manager_;
};
} // namespace WebPtt::Core
