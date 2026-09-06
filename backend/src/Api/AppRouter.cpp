#include "AppRouter.hpp"
#include "Stt/Client.hpp"
#include "Utils.hpp"
#include "Utils/variant.hpp"
#include "WebRtc/Session.hpp"
#include <WebRtc/BridgeManager.hpp>
#include <WebRtc/Bridge.hpp>
#include <spdlog/spdlog.h>
#include <variant>

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

std::string error_message(const Stt::TranscribeError& error) {
    return std::visit(
        Utils::Overloaded{
            [](const std::string& message) { return message; },
            [](const Stt::TranscribeErrorResponse& response) { return response.detail.error_; },
        },
        error);
}

void log_transcription(const Stt::TranscribeResponse& response) {
    const auto log_value = [&response](const auto& value) {
        spdlog::info(
            "STT succeeded: raw_text='{}', command='{}', value={}, processing_time_ms={}, "
            "audio_duration_ms={}, input_samples={}, whisper_samples={}",
            response.raw_text_,
            response.command_,
            value,
            response.processing_time_ms_,
            response.audio_duration_ms_,
            response.input_samples_,
            response.whisper_samples_);
    };

    std::visit(
        Utils::Overloaded{
            [log_value](double value) { log_value(value); },
            [log_value](const std::string& value) { log_value(value); },
        },
        response.value_);
}
} // namespace

AppRouter::AppRouter(
    std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager,
    std::shared_ptr<Stt::Client> stt_client)
    : bridge_manager_(std::move(peer_connection_manager))
    , stt_client_(std::move(stt_client)) {}

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

    router_.add_route("/ptt/start", Http::verb::post, [this](const Http::request<Http::string_body>& request) {
        return ptt_start(request);
    });

    router_.add_route(
        "/ptt/stop",
        Http::verb::post,
        [this](const Http::request<Http::string_body>& request, ResponseHandler handler) {
            ptt_stop(request, std::move(handler));
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

Http::response<Http::string_body> AppRouter::ptt_start(const Http::request<Http::string_body>& request) {
    auto parsed = Utils::parse_json<PttStartRequest>(request.body());
    if (!parsed) {
        return make_error_response(request, Http::status::bad_request, parsed.error());
    }

    auto bridge = bridge_manager_.find_bridge_by_id(parsed->bridge_id_);
    if (!bridge) {
        return make_error_response(request, Http::status::not_found, "bridge was not found");
    }

    auto session = bridge->find_session(parsed->session_id_);
    if (!session) {
        return make_error_response(request, Http::status::not_found, "session was not found");
    }

    session->set_buffering_command(true);
    return make_json_response(request, Http::status::ok, PttStartResponse{.success_ = true});
}

void AppRouter::ptt_stop(const Http::request<Http::string_body>& request, ResponseHandler handler) {
    auto parsed = Utils::parse_json<PttStopRequest>(request.body());
    if (!parsed) {
        handler(make_error_response(request, Http::status::bad_request, parsed.error()));
        return;
    }

    auto bridge = bridge_manager_.find_bridge_by_id(parsed->bridge_id_);
    if (!bridge) {
        handler(make_error_response(request, Http::status::not_found, "bridge was not found"));
        return;
    }

    auto session = bridge->find_session(parsed->session_id_);
    if (!session) {
        handler(make_error_response(request, Http::status::not_found, "session was not found"));
        return;
    }

    session->set_buffering_command(false);
    auto samples = session->take_command_buffer();
    if (samples.empty()) {
        handler(make_json_response(request, Http::status::ok, PttStopResponse{.success_ = true}));
        return;
    }
    stt_client_->transcribe(
        samples,
        [request, handler = std::move(handler)](Stt::Client::TranscribeResult transcription) {
            if (!transcription) {
                handler(make_error_response(request, Http::status::bad_gateway, error_message(transcription.error())));
                return;
            }

            log_transcription(*transcription);
            handler(make_json_response(request, Http::status::ok, PttStopResponse{.success_ = true}));
        });
}

void AppRouter::route(const Http::request<Http::string_body>& request, ResponseHandler handler) {
    router_.route(request, std::move(handler));
}

Http::response<Http::string_body> AppRouter::is_alive(const Http::request<Http::string_body>& request) {
    return make_empty_response(request, Http::status::ok);
}
} // namespace WebPtt::Api
