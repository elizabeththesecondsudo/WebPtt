#include "Listener.hpp"

namespace WebPtt::Api {
void Listener::listen() {
    acceptor_.async_accept([this](boost::system::error_code error, Tcp::socket socket) {
        if (error) {
            // Handle error (e.g., log it)
            return;
        }

        listen(); // Continue accepting new connections
    });
}
} // namespace WebPtt::Api