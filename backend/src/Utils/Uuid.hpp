#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace WebPtt::Utils {
inline boost::uuids::uuid generate_uuid() {
    boost::uuids::random_generator generator;
    return generator();
}
} // namespace WebPtt::Utils
