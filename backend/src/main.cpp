#include "Api/Listener.hpp"
#include "Core/Settings.hpp"
#include "Stt/Client.hpp"
#include "Utils/Net.hpp"
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

    auto socket_res = WebPtt::Utils::create_socket(io_context.get_executor(), stt.address_, stt.port_);
    if(!socket_res) {
        spdlog::critical("Failed to open an socket: {}", socket_res.error());
        return EXIT_FAILURE;
    }

    auto stt_client = std::make_shared<WebPtt::Stt::Client>(std::move(socket_res.value()));

    WebPtt::Api::Listener listener(std::move(acceptor_res.value()));
    listener.listen();

    io_context.run();

    return EXIT_SUCCESS;
}
