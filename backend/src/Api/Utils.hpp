#pragma once

#include "Types.hpp"

namespace WebPtt::Api {
Http::response<Http::string_body> make_basic_api_response(
    const Http::request<Http::string_body>& request,
    Http::status status);
} // namespace WebPtt::Api
