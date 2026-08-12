#include "Api/Listener.hpp"
#include "Api/Utils.hpp"

int main() {
    boost::asio::io_context io_context;
    WebPtt::Api::Listener listener(WebPtt::Api::create_acceptor(io_context.get_executor(), WebPtt::Api::Tcp::endpoint{boost::asio::ip::make_address("127.0.0.1"), 8080}).value());
    listener.listen();
    io_context.run();
    return 0;
}