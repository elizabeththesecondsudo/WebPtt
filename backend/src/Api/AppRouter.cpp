#include "AppRouter.hpp"
#include "Utils.hpp"
#include "WebRtc/Bridge.hpp"

namespace WebPtt::Api {
namespace {
Http::status status_for(WebRtc::BridgeErrorCode error) {
    switch (error) {
    case WebRtc::BridgeErrorCode::kSessionNotFound: return Http::status::not_found;
    case WebRtc::BridgeErrorCode::kSessionBusy: return Http::status::conflict;
    case WebRtc::BridgeErrorCode::kInvalidRequest: return Http::status::bad_request;
    }

    return Http::status::bad_request;
}
} // namespace

AppRouter::AppRouter(std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager)
    : bridge_manager_(std::move(peer_connection_manager)) {}

void AppRouter::register_routes() {
    router_.add_route("/isAlive", Http::verb::get, [this](const Http::request<Http::string_body>& request) {
        return is_alive(request);
    });
    router_.add_route("/createSession", Http::verb::post, [this](const Http::request<Http::string_body>& request) {
        return create_session(request);
    });
    router_.add_route("/deleteSession", Http::verb::post, [this](const Http::request<Http::string_body>& request) {
        return delete_session(request);
    });
    router_.add_route("/sessionStatus", Http::verb::post, [this](const Http::request<Http::string_body>& request) {
        return session_status(request);
    });
}

Http::response<Http::string_body> AppRouter::create_session(const Http::request<Http::string_body>& request) {
    const auto parsed = Utils::parse_json<CreateSessionRequest>(request.body());
    if (!parsed) {
        return make_error_response(request, Http::status::bad_request, parsed.error());
    }

    const auto bridge = bridge_manager_.create_bridge(parsed->session_id_, parsed->target_session_id_);
    if (!bridge) {
        return make_error_response(request, status_for(bridge.error().code_), bridge.error().message_);
    }

    return make_json_response(
        request,
        Http::status::created,
        CreateSessionResponse{
            .bridge_id_ = (*bridge)->id(),
            .session_id_ = parsed->session_id_,
            .target_session_id_ = parsed->target_session_id_,
        });
}

Http::response<Http::string_body> AppRouter::delete_session(const Http::request<Http::string_body>& request) {
    const auto parsed = Utils::parse_json<DeleteSessionRequest>(request.body());
    if (!parsed) {
        return make_error_response(request, Http::status::bad_request, parsed.error());
    }
    if (parsed->bridge_id_.empty()) {
        return make_error_response(request, Http::status::bad_request, "bridge_id_ must not be empty");
    }

    if (!bridge_manager_.remove_bridge(parsed->bridge_id_)) {
        return make_error_response(request, Http::status::not_found, "bridge was not found");
    }

    return make_empty_response(request, Http::status::no_content);
}

Http::response<Http::string_body> AppRouter::session_status(const Http::request<Http::string_body>& request) {
    const auto parsed = Utils::parse_json<SessionStatusRequest>(request.body());
    if (!parsed || parsed->session_id_.empty()) {
        return make_error_response(
            request,
            Http::status::bad_request,
            parsed ? "session_id_ must not be empty" : parsed.error());
    }

    const auto bridge = bridge_manager_.find_bridge(parsed->session_id_);
    return make_json_response(
        request,
        Http::status::ok,
        SessionStatusResponse{
            .active_ = bridge != nullptr,
            .bridge_id_ = bridge ? bridge->id() : std::string{},
            .source_session_id_ = bridge ? bridge->source_id() : std::string{},
            .target_session_id_ = bridge ? bridge->target_id() : std::string{},
        });
}

Http::response<Http::string_body> AppRouter::route(const Http::request<Http::string_body>& request) {
    return router_.route(request);
}

Http::response<Http::string_body> AppRouter::is_alive(const Http::request<Http::string_body>& request) {
    return make_empty_response(request, Http::status::ok);
}
} // namespace WebPtt::Api
