#include "Utils.hpp"

namespace WebPtt::Api {
Http::response<Http::string_body> make_basic_api_response(
    const Http::request<Http::string_body>& request,
    Http::status status) {
    Http::response<Http::string_body> response{status, request.version()};
    response.set(Http::field::server, "WebPtt");
    response.set(Http::field::content_type, "application/json");
    response.set(Http::field::access_control_allow_origin, "*");
    response.keep_alive(request.keep_alive());
    return response;
}

Http::response<Http::string_body> make_error_response(
    const Http::request<Http::string_body>& request,
    Http::status status,
    std::string message) {
    return make_json_response(request, status, ErrorResponse{.error_ = std::move(message)});
}

Http::response<Http::string_body> make_empty_response(
    const Http::request<Http::string_body>& request,
    Http::status status) {
    auto response = make_basic_api_response(request, status);
    response.prepare_payload();
    return response;
}
} // namespace WebPtt::Api
