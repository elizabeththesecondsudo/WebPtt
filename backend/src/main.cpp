#include "Api/Listener.hpp"
#include "Api/WebSocketManager.hpp"
#include "Core/Coordinator.hpp"
#include "Core/Settings.hpp"
#include "Utils/Net.hpp"
#include "WebRtc/PeerConnectionManager.hpp"
#include <spdlog/spdlog.h>
#include <cstdlib>

int main() {
    boost::asio::io_context io_context;

    auto settings_res = WebPtt::Core::load_settings("settings.toml");
    if (!settings_res) {
        spdlog::critical("failed to load settings: {}", settings_res.error());
        return EXIT_FAILURE;
    }

    auto settings = std::move(settings_res.value());
    const auto& [media_plane, stt] = settings;

    auto acceptor_res = WebPtt::Utils::create_acceptor(
        io_context.get_executor(),
        media_plane.listening_address_,
        media_plane.listening_port_);

    if (!acceptor_res) {
        spdlog::critical("Failed to open an acceptor: {}", acceptor_res.error());
        return EXIT_FAILURE;
    }

    auto websocket_manager = std::make_shared<WebPtt::Api::WebSocketManager>();
    auto peer_connection_manager = std::make_shared<WebPtt::WebRtc::PeerConnectionManager>();
    auto coordinator =
        std::make_shared<WebPtt::Core::Coordinator>(websocket_manager, peer_connection_manager);

    WebPtt::Api::Listener listener(
        std::move(acceptor_res.value()),
        [coordinator](WebPtt::Tcp::socket socket, WebPtt::Api::Http::request<WebPtt::Api::Http::string_body> request) {
            coordinator->attatch(std::move(socket), std::move(request));
        });
    listener.listen();

    io_context.run();

    return EXIT_SUCCESS;
}
