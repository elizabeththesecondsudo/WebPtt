#include "AppRouter.hpp"
#include "Utils.hpp"
#include "Utils/Json.hpp"
#include "WebRtc/Bridge.hpp"

namespace WebPtt::Api {
struct CreateSessionRequest {
    std::string session_id_;
    std::string target_session_id_;
};

struct CreateSessionResponse {
    std::string bridge_id_;
    std::string session_id_;
    std::string target_session_id_;
};

struct ErrorResponse {
    std::string error_;
};

AppRouter::AppRouter(std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager)
    : bridge_manager_(std::move(peer_connection_manager)) {}

void AppRouter::register_routes() {
    router_.add_route("/isAlive", Http::verb::get, [this](const Http::request<Http::string_body>& request) {
        return is_alive(request);
    });
    router_.add_route("/createSession", Http::verb::post, [this](const Http::request<Http::string_body>& request) {
        return create_session(request);
    });
}

Http::response<Http::string_body> AppRouter::create_session(const Http::request<Http::string_body>& request) {
    const auto parsed = Utils::parse_json<CreateSessionRequest>(request.body());
    if (!parsed) {
        auto response = make_basic_api_response(request, Http::status::bad_request);
        response.body() = Utils::serialize_json(ErrorResponse{.error_ = parsed.error()}).value_or("{}");
        response.prepare_payload();
        return response;
    }

    const auto bridge = bridge_manager_.create_bridge(parsed->session_id_, parsed->target_session_id_);
    if (!bridge) {
        const auto status = bridge.error().code_ == WebRtc::BridgeErrorCode::session_not_found ? Http::status::not_found
                            : bridge.error().code_ == WebRtc::BridgeErrorCode::session_busy    ? Http::status::conflict
                                                                                            : Http::status::bad_request;
        auto response = make_basic_api_response(request, status);
        response.body() = Utils::serialize_json(ErrorResponse{.error_ = bridge.error().message_}).value_or("{}");
        response.prepare_payload();
        return response;
    }

    auto response = make_basic_api_response(request, Http::status::created);
    response.body() = Utils::serialize_json(
                          CreateSessionResponse{
                              .bridge_id_ = (*bridge)->id(),
                              .session_id_ = parsed->session_id_,
                              .target_session_id_ = parsed->target_session_id_,
                          })
                          .value_or("{}");
    response.prepare_payload();
    return response;
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
