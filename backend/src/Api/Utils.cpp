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
} // namespace WebPtt::Api
