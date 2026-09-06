#pragma once

#include <functional>
#include <string_view>
#include <unordered_map>

#include "Types.hpp"

namespace WebPtt::Api {

using ResponseHandler = std::function<void(Http::response<Http::string_body>)>;
using SyncHandler = std::function<Http::response<Http::string_body>(const Http::request<Http::string_body>&)>;
using AsyncHandler = std::function<void(const Http::request<Http::string_body>&, ResponseHandler)>;

struct ApiEndpoint {
    std::string_view target_;
    Http::verb method_;

    bool operator==(const ApiEndpoint&) const = default;
};

struct ApiEndpointHash {
    std::size_t operator()(const ApiEndpoint& endpoint) const noexcept {
        const auto target_hash = std::hash<std::string_view>{}(endpoint.target_);

        const auto method_hash = std::hash<unsigned>{}(static_cast<unsigned>(endpoint.method_));

        return target_hash ^ (method_hash << 1); // NOLINT
    }
};

class Router {
public:
    void add_route(std::string_view target, Http::verb method, SyncHandler handler);
    void add_route(std::string_view target, Http::verb method, AsyncHandler handler);
    void route(const Http::request<Http::string_body>& request, ResponseHandler handler);

private:
    std::unordered_map<ApiEndpoint, AsyncHandler, ApiEndpointHash> routes_;
};

} // namespace WebPtt::Api
