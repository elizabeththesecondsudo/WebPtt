#pragma once

#include "Types.hpp"

namespace WebPtt::Api {
class Listener {
public:
    explicit Listener(Tcp::acceptor acceptor)
        : acceptor_(std::move(acceptor)) {}

    void listen();

private:
    Tcp::acceptor acceptor_;
};
} // namespace WebPtt::Api