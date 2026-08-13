#include "Listener.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include "Session.hpp"

namespace MediaPlane::Api {
Listener::Listener(Tcp::acceptor acceptor)
    : acceptor_(std::move(acceptor))
    , router_(std::make_shared<AppRouter>()) {
    router_->register_routes();
}

void Listener::listen() {
    acceptor_.async_accept([this](boost::system::error_code error, Tcp::socket socket) {
        if (error) {
            spdlog::error("Error accepting connection: {}", error.message());
            return;
        }

        spdlog::info(
            "Got connection from: {}:{}",
            socket.remote_endpoint().address().to_string(),
            socket.remote_endpoint().port());

        std::make_shared<Session>(std::move(socket), router_)->start();
        listen(); // Continue accepting new connections
    });
}
} // namespace MediaPlane::Api
