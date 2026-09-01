#pragma once

#include "Types.hpp"
#include "Utils/Json.hpp"

#include <string>
#include <utility>

namespace WebPtt::Api {
Http::response<Http::string_body> make_basic_api_response(
    const Http::request<Http::string_body>& request,
    Http::status status);

template <typename Body>
Http::response<Http::string_body>
make_json_response(const Http::request<Http::string_body>& request, Http::status status, const Body& body) {
    auto response = make_basic_api_response(request, status);
    response.body() = Utils::serialize_json(body).value_or("{}");
    response.prepare_payload();
    return response;
}

Http::response<Http::string_body>
make_error_response(const Http::request<Http::string_body>& request, Http::status status, std::string message);

Http::response<Http::string_body> make_empty_response(
    const Http::request<Http::string_body>& request,
    Http::status status);
} // namespace WebPtt::Api
