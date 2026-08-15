#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>

namespace WebPtt::Api {
    using Tcp = boost::asio::ip::tcp;
    using Executor = boost::asio::any_io_executor;
    namespace Http = boost::beast::http;
}
