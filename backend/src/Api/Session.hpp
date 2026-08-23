#pragma once

#include <memory>
#include "AppRouter.hpp"
#include "Core/Types.hpp"
#include "Types.hpp"
#include <boost/beast/core/flat_buffer.hpp>

namespace WebPtt::Api {
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(Tcp::socket socket, std::shared_ptr<AppRouter> router);

    void start();

private:
    void do_read();
    void do_write(Http::response<Http::string_body> response);

    Tcp::socket socket_;
    Tcp::endpoint remote_endpoint_;
    boost::beast::flat_buffer buffer_;
    Http::request<Http::string_body> request_;
    Http::response<Http::string_body> response_;
    std::shared_ptr<AppRouter> router_;
};
} // namespace WebPtt::Api
