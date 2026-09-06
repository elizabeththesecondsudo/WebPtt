#include "Router.hpp"
#include "Utils.hpp"

namespace WebPtt::Api {
void Router::add_route(std::string_view target, Http::verb method, SyncHandler handler) {
    add_route(target, method, [handler = std::move(handler)](const auto& request, ResponseHandler respond) {
        respond(handler(request));
    });
}

void Router::add_route(std::string_view target, Http::verb method, AsyncHandler handler) {
    ApiEndpoint endpoint{.target_ = target, .method_ = method};
    routes_[endpoint] = std::move(handler);
}

void Router::route(const Http::request<Http::string_body>& request, ResponseHandler handler) {
    ApiEndpoint endpoint{.target_ = request.target(), .method_ = request.method()};
    auto itr = routes_.find(endpoint);
    if (itr != routes_.end()) {
        itr->second(request, std::move(handler));
        return;
    }

    handler(make_basic_api_response(request, Http::status::not_found));
}
} // namespace WebPtt::Api
