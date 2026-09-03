#pragma once

namespace WebPtt::Utils {
template <typename... Visitors>
struct Overloaded : Visitors... {
    using Visitors::operator()...;
};

template <typename... Visitors>
Overloaded(Visitors...) -> Overloaded<Visitors...>;
} // namespace WebPtt::Utils
