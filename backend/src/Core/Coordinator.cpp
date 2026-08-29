#include "Coordinator.hpp"

#include "Api/WebSocketManager.hpp"
#include "Api/WebSocketSession.hpp"
#include "WebRtc/Messages.hpp"
#include "Utils/Json.hpp"
#include "WebRtc/PeerConnectionManager.hpp"
#include "WebRtc/Session.hpp"
#include <boost/asio/post.hpp>
#include <spdlog/spdlog.h>

namespace WebPtt::Core {
Coordinator::Coordinator(
    std::shared_ptr<Api::WebSocketManager> websocket_manager,
    std::shared_ptr<WebRtc::PeerConnectionManager> peer_connection_manager)
    : websocket_manager_(std::move(websocket_manager))
    , peer_connection_manager_(std::move(peer_connection_manager)) {}

void Coordinator::attatch(Tcp::socket socket, Api::Http::request<Api::Http::string_body> request) {
    auto peer_session_res = peer_connection_manager_->create_session();
    if (!peer_session_res) {
        spdlog::error("Failed to create peer connection for WebSocket client: {}", peer_session_res.error());
        return;
    }

    auto peer_session = std::move(peer_session_res.value());
    auto session = std::make_shared<Api::WebSocketSession>(std::move(socket));

    spdlog::debug("Coordinator registering WebSocket session");
    websocket_manager_->add_session(session);

    configure_peer_connection(peer_session, session);
    start_websocket_session(peer_session, session, std::move(request));
}

void Coordinator::configure_peer_connection(
    const std::shared_ptr<WebRtc::Session>& peer_session,
    const std::shared_ptr<Api::WebSocketSession>& websocket_session) {
    auto weak_websocket_session = std::weak_ptr<Api::WebSocketSession>(websocket_session);
    auto weak_peer_session = std::weak_ptr<WebRtc::Session>(peer_session);

    peer_session->on_audio(
        [weak_peer_session](rtc::binary opus_frame, std::uint32_t) {
            if (auto session = weak_peer_session.lock()) {
                static_cast<void>(session->send_audio(std::move(opus_frame)));
            }
        });

    peer_session->configure(
        [weak_websocket_session](const rtc::Description& description) {
            auto websocket_session = weak_websocket_session.lock();
            if (!websocket_session) {
                return;
            }

            auto message = Utils::serialize_json(
                SessionDescriptionMessage{
                    .type_ = description.typeString(),
                    .sdp_ = std::string(description),
                });
            if (!message) {
                spdlog::error("Failed to serialize local SDP: {}", message.error());
                return;
            }

            boost::asio::post(
                websocket_session->executor(),
                [weak_websocket_session, message = std::move(message.value())]() mutable {
                    if (auto session = weak_websocket_session.lock()) {
                        session->write(std::move(message));
                    }
                });
        },
        [weak_websocket_session](const rtc::Candidate& candidate) {
            auto websocket_session = weak_websocket_session.lock();
            if (!websocket_session) {
                return;
            }

            auto message = Utils::serialize_json(
                IceCandidateMessage{
                    .candidate_ = std::string(candidate),
                    .sdp_mid_ = candidate.mid(),
                });
            if (!message) {
                spdlog::error("Failed to serialize local ICE candidate: {}", message.error());
                return;
            }

            boost::asio::post(
                websocket_session->executor(),
                [weak_websocket_session, message = std::move(message.value())]() mutable {
                    if (auto session = weak_websocket_session.lock()) {
                        session->write(std::move(message));
                    }
                });
        });
}

void Coordinator::start_websocket_session(
    const std::shared_ptr<WebRtc::Session>& peer_session,
    const std::shared_ptr<Api::WebSocketSession>& websocket_session,
    Api::Http::request<Api::Http::string_body> request) {
    websocket_session->start(
        std::move(request),
        peer_session->id(),
        [](const std::shared_ptr<Api::WebSocketSession>& connected_session) {
            auto message = Utils::serialize_json(
                SessionCreatedMessage{
                    .session_id_ = connected_session->id(),
                });
            if (!message) {
                spdlog::error("Failed to serialize peer session ID: {}", message.error());
                return;
            }

            connected_session->write(std::move(message.value()));
            spdlog::debug("Sent peer session ID {} to WebSocket client", connected_session->id());
        },
        [peer_session](const std::shared_ptr<Api::WebSocketSession>& websocket_session, const std::string& payload) {
            auto description = Utils::parse_json<SessionDescriptionMessage>(payload);
            if (description && (description->type_ == "offer" || description->type_ == "answer")) {
                auto result = peer_session->set_remote_description(description->sdp_, description->type_);
                if (!result) {
                    spdlog::error(
                        "Failed to apply remote SDP for peer {}: {}",
                        websocket_session->id(),
                        result.error());
                    return;
                }

                spdlog::debug("Applied remote {} for peer {}", description->type_, websocket_session->id());
                return;
            }

            auto candidate = Utils::parse_json<IceCandidateMessage>(payload);
            if (candidate && (candidate->type_ == "candidate" || candidate->type_ == "ice_candidate")) {
                auto result = peer_session->add_remote_candidate(candidate->candidate_, candidate->sdp_mid_);
                if (!result) {
                    spdlog::error(
                        "Failed to apply remote ICE candidate for peer {}: {}",
                        websocket_session->id(),
                        result.error());
                    return;
                }

                spdlog::debug("Applied remote ICE candidate for peer {}", websocket_session->id());
                return;
            }

            spdlog::warn("Ignoring invalid signaling message from peer {}", websocket_session->id());
        },
        [this](const std::shared_ptr<Api::WebSocketSession>& closed_session) { detach(closed_session); });
}

void Coordinator::detach(const std::shared_ptr<Api::WebSocketSession>& session) {
    spdlog::debug("Coordinator removing WebSocket session");

    if (const auto peer = peer_connection_manager_->find_session(session->id()); peer != nullptr) {
        peer_connection_manager_->remove_session(peer->id());
    }

    websocket_manager_->remove_session(session);
}
} // namespace WebPtt::Core
