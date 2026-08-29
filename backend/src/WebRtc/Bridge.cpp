#include "Bridge.hpp"

#include "Session.hpp"
#include "Utils/Uuid.hpp"

#include <spdlog/spdlog.h>
#include <utility>

namespace WebPtt::WebRtc {
Bridge::Bridge(std::shared_ptr<Session> first, std::shared_ptr<Session> second)
    : id_(Utils::generate_uuid())
    , first_(std::move(first))
    , second_(std::move(second)) {}

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

const std::string& Bridge::id() const noexcept {
    return id_;
}

bool Bridge::active() const noexcept {
    return !first_.expired() && !second_.expired();
}
} // namespace WebPtt::WebRtc
