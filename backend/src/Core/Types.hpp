#pragma once

#include <boost/asio/ip/tcp.hpp>

namespace WebPtt {
using Tcp = boost::asio::ip::tcp;
using Executor = boost::asio::any_io_executor;
} // namespace WebPtt