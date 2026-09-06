#include "Client.hpp"

#include "Core/Types.hpp"
#include "Utils/Json.hpp"

#include <expected>
#include <span>
#include <utility>

namespace WebPtt::Stt {
namespace {
void assign_basic_fields(Api::Http::request<Api::Http::vector_body<std::byte>>& request, const std::string& host) {
    constexpr unsigned kHttpVersion = 11;
    request.method(boost::beast::http::verb::post);
    request.target("/transcribe");
    request.version(kHttpVersion);
    request.set(boost::beast::http::field::host, host);
    request.set(boost::beast::http::field::content_type, "application/octet-stream");
    request.keep_alive(false);
}
} // namespace

Client::Client(const Executor& executor, const Tcp::endpoint& endpoint)
    : socket_(executor), endpoint_(endpoint) {}

void Client::transcribe(std::span<const float> samples, TranscribeHandler handler) {
    // Each transcription owns its connection and HTTP state. An idle service
    // connection may have closed, and multiple users can transcribe at once.
    auto operation = std::make_shared<Client>(socket_.get_executor(), endpoint_);
    assign_basic_fields(operation->request_, endpoint_.address().to_string());
    const auto sample_bytes = std::as_bytes(samples);
    operation->request_.body().assign(sample_bytes.begin(), sample_bytes.end());
    operation->request_.prepare_payload();
    operation->socket_.async_connect(
        endpoint_,
        [operation, handler = std::move(handler)](boost::system::error_code error) mutable {
            if (error) {
                handler(std::unexpected("Could not connect to speech-to-text service: " + error.message()));
                return;
            }
            operation->write(std::move(handler));
        });
}

void Client::write(TranscribeHandler handler) {
    boost::beast::http::async_write(
        socket_,
        request_,
        [self = shared_from_this(),
         handler = std::move(handler)](boost::system::error_code error, std::size_t) mutable {
            if (error) {
                handler(std::unexpected(error.message()));
                return;
            }
            self->on_write(std::move(handler));
        });
}

void Client::on_write(TranscribeHandler handler) {
    response_ = {};
    boost::beast::http::async_read(
        socket_,
        buffer_,
        response_,
        [self = shared_from_this(),
         handler = std::move(handler)](boost::system::error_code error, std::size_t) mutable {
            if (error) {
                handler(std::unexpected(error.message()));
                return;
            }
            self->on_read(std::move(handler));
        });
}

void Client::on_read(TranscribeHandler handler) { // NOLINT
    if (response_.result() != Api::Http::status::ok) {
        auto error_response = Utils::parse_json<TranscribeErrorResponse>(response_.body());
        if (!error_response) {
            handler(std::unexpected(error_response.error()));
            return;
        }

        handler(std::unexpected(std::move(error_response.value())));
        return;
    }

    auto response = Utils::parse_json<TranscribeResponse>(response_.body());
    if (!response) {
        handler(std::unexpected(response.error()));
        return;
    }

    handler(std::move(response.value()));
}
} // namespace WebPtt::Stt
