#pragma once

#include "Router.hpp"

#include "WebRtc/BridgeManager.hpp"
#include "boost/beast/http/message_fwd.hpp"
#include "boost/beast/http/string_body_fwd.hpp"

#include <memory>

namespace WebPtt::WebRtc {
class PeerConnectionManager;
} // namespace WebPtt::WebRtc

namespace WebPtt::Stt {
class Client;
} // namespace WebPtt::Stt

namespace WebPtt::Api {
class AppRouter {
public:
    AppRouter(
        std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager,
        std::shared_ptr<Stt::Client> stt_client);
    void register_routes();
    void route(const Http::request<Http::string_body>& request, ResponseHandler handler);

private:
    static Http::response<Http::string_body> is_alive(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> create_session(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> delete_session(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> session_status(const Http::request<Http::string_body>& request);
    Http::response<Http::string_body> ptt_start(const Http::request<Http::string_body>& request);
    void ptt_stop(const Http::request<Http::string_body>& request, ResponseHandler handler);

    Router router_;
    WebRtc::BridgeManager bridge_manager_;
    std::shared_ptr<Stt::Client> stt_client_;
};
} // namespace WebPtt::Api
