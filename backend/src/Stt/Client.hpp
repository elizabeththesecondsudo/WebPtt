#pragma once

#include "Api/Types.hpp"
#include "Core/Types.hpp"
#include "Types.hpp"

#include <boost/beast/core/flat_buffer.hpp>
#include <cstddef>
#include <expected>
#include <functional>
#include <memory>
#include <span>

namespace WebPtt::Stt {
class Client : public std::enable_shared_from_this<Client> {
public:
    using TranscribeResult = std::expected<TranscribeResponse, TranscribeError>;
    using TranscribeHandler = std::function<void(TranscribeResult)>;

    explicit Client(Tcp::socket socket);

    void transcribe(std::span<const float> samples, TranscribeHandler handler);

private:
    void on_write(TranscribeHandler handler);
    void on_read(TranscribeHandler handler);

    Tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    Api::Http::request<Api::Http::vector_body<std::byte>> request_;
    Api::Http::response<Api::Http::string_body> response_;
};
} // namespace WebPtt::Stt
