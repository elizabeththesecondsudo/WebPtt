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
    request.keep_alive(true);
}
} // namespace

Client::Client(Tcp::socket socket)
    : socket_(std::move(socket)) {}

void Client::transcribe(std::span<const float> samples, TranscribeHandler handler) {
    boost::system::error_code error;
    auto remote_endpoint = socket_.remote_endpoint(error);
    if (error) {
        handler(std::unexpected(error.message()));
        return;
    }
    auto host = remote_endpoint.address().to_string();

    request_ = {};
    assign_basic_fields(request_, host);
    const auto sample_bytes = std::as_bytes(samples);
    request_.body().assign(sample_bytes.begin(), sample_bytes.end());
    request_.prepare_payload();

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
