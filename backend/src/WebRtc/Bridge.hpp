#pragma once

#include <memory>
#include <string>

namespace WebPtt::WebRtc {
class Session;

class Bridge {
public:
    Bridge(std::shared_ptr<Session> first, std::shared_ptr<Session> second);

    void connect();
    void disconnect();
    [[nodiscard]] const std::string& id() const noexcept;
    [[nodiscard]] bool active() const noexcept;

private:
    std::string id_;
    std::weak_ptr<Session> first_;
    std::weak_ptr<Session> second_;
};
} // namespace WebPtt::WebRtc
