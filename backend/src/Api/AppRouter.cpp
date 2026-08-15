#include "AppRouter.hpp"
#include "Utils.hpp"

namespace WebPtt::Api {
void AppRouter::register_routes() {
    router_.add_route("/isAlive", Http::verb::get, [this](const Http::request<Http::string_body>& request) {
        return is_alive(request);
    });
}

Http::response<Http::string_body> AppRouter::route(const Http::request<Http::string_body>& request) {
    return router_.route(request);
}

Http::response<Http::string_body> AppRouter::is_alive(const Http::request<Http::string_body>& request) {
    auto response = make_basic_api_response(request, Http::status::ok);
    response.prepare_payload();
    return response;
}
} // namespace WebPtt::Api
