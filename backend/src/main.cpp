#include "Api/Listener.hpp"
#include "Core/Settings.hpp"
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

    WebPtt::Api::Listener listener(std::move(acceptor_res.value()));
    listener.listen();

    io_context.run();

    return EXIT_SUCCESS;
}
