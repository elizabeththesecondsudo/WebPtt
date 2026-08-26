#pragma once

#include <memory>
#include <vector>

namespace WebPtt::Api {
class WebSocketSession;

class WebSocketManager {
public:
    void add_session(std::shared_ptr<WebSocketSession> session);
    void remove_session(const std::shared_ptr<WebSocketSession>& session);

private:
    std::vector<std::shared_ptr<WebSocketSession>> sessions_;
};
} // namespace WebPtt::Api
