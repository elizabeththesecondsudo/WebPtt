#pragma once

#include "AppRouter.hpp"
#include "Types.hpp"

namespace MediaPlane::Api {
class Listener {
public:
    explicit Listener(Tcp::acceptor acceptor);

    void listen();

private:
    Tcp::acceptor acceptor_;
    std::shared_ptr<AppRouter> router_;
};
} // namespace MediaPlane::Api
