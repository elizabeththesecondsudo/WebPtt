#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include "boost/uuid/uuid_io.hpp"

namespace WebPtt::Utils {
inline std::string generate_uuid() {
    boost::uuids::random_generator generator;
    return boost::uuids::to_string(generator());
}
} // namespace WebPtt::Utils
