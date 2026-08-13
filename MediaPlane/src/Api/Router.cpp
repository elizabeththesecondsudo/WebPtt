#include "Router.hpp"
#include "Utils.hpp"

namespace MediaPlane::Api {
void Router::add_route(std::string_view target, Http::verb method, Handler handler) {
    ApiEndpoint endpoint{.target_ = target, .method_ = method};
    routes_[endpoint] = std::move(handler);
}

Http::response<Http::string_body> Router::route(const Http::request<Http::string_body>& request) {
    ApiEndpoint endpoint{.target_ = request.target(), .method_ = request.method()};
    auto itr = routes_.find(endpoint);
    if (itr != routes_.end()) {
        return itr->second(request);
    }

    return make_basic_api_response(request, Http::status::not_found);
}
} // namespace MediaPlane::Api
