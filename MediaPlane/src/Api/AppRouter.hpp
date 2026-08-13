#pragma once

#include "Router.hpp"


namespace MediaPlane::Api {
class AppRouter {
public:
    void register_routes();
    Http::response<Http::string_body> route(const Http::request<Http::string_body>& request);

private:
    static Http::response<Http::string_body> is_alive(const Http::request<Http::string_body>& request);
    Router router_;
};
} // namespace MediaPlane::Api
