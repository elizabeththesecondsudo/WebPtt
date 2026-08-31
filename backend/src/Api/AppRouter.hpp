#pragma once

#include "Router.hpp"

#include "WebRtc/BridgeManager.hpp"

#include <memory>

namespace WebPtt::WebRtc {
class PeerConnectionManager;
}


namespace WebPtt::Api {
class AppRouter {
public:
    explicit AppRouter(std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager);
    void register_routes();
    Http::response<Http::string_body> route(const Http::request<Http::string_body>& request);

private:
    static Http::response<Http::string_body> is_alive(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> create_session(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> delete_session(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> session_status(const Http::request<Http::string_body>& request);

    Router router_;
    WebRtc::BridgeManager bridge_manager_;
};
} // namespace WebPtt::Api
