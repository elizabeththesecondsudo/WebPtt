#pragma once

#include <functional>
#include <string_view>
#include <unordered_map>

#include "Types.hpp"

namespace MediaPlane::Api {

using Handler = std::function<Http::response<Http::string_body>(const Http::request<Http::string_body>&)>;

struct ApiEndpoint {
    std::string_view target_;
    Http::verb method_;

    bool operator==(const ApiEndpoint&) const = default;
};

struct ApiEndpointHash {
    std::size_t operator()(const ApiEndpoint& endpoint) const noexcept {
        const auto target_hash = std::hash<std::string_view>{}(endpoint.target_);

        const auto method_hash = std::hash<unsigned>{}(static_cast<unsigned>(endpoint.method_));

        return target_hash ^ (method_hash << 1); //NOLINT
    }
};

class Router {
public:
    void add_route(std::string_view target, Http::verb method, Handler handler);
    Http::response<Http::string_body> route(const Http::request<Http::string_body>& request);

private:
    std::unordered_map<ApiEndpoint, Handler, ApiEndpointHash> routes_;
};

} // namespace MediaPlane::Api
