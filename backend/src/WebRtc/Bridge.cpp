#include "Bridge.hpp"

#include "Session.hpp"
#include "Utils/Uuid.hpp"

#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::WebRtc {
Bridge::Bridge(std::shared_ptr<Session> first, std::shared_ptr<Session> second)
    : id_(Utils::generate_uuid())
    , first_(first)
    , second_(second) {}


void Bridge::connect() {
    const auto first = first_.lock();
    const auto second = second_.lock();
    if (!first || !second) {
        return;
    }

    first->on_audio([target = std::weak_ptr<Session>(second)](rtc::binary frame, std::uint32_t) {
        if (const auto session = target.lock()) {
            static_cast<void>(session->send_audio(std::move(frame)));
        }
    });
    second->on_audio([target = std::weak_ptr<Session>(first)](rtc::binary frame, std::uint32_t) {
        if (const auto session = target.lock()) {
            static_cast<void>(session->send_audio(std::move(frame)));
        }
    });

    spdlog::info("Created bridge {} between peers {} and {}", id_, first->id(), second->id());
}

void Bridge::disconnect() {
    if (first_.expired() && second_.expired()) {
        return;
    }

    if (const auto first = first_.lock()) {
        first->on_audio({});
    }
    if (const auto second = second_.lock()) {
        second->on_audio({});
    }

    first_.reset();
    second_.reset();
    spdlog::info("Disconnected bridge {}", id_);
}

const std::string& Bridge::id() const noexcept {
    return id_;
}

std::string Bridge::source_id() const {
    const auto source = first_.lock();
    return source ? source->id() : std::string{};
}

std::string Bridge::target_id() const {
    const auto target = second_.lock();
    return target ? target->id() : std::string{};
}

bool Bridge::active() const noexcept {
    return !first_.expired() && !second_.expired();
}
} // namespace WebPtt::WebRtc
