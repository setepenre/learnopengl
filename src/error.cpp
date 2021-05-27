#include <algorithm>
#include <sstream>

#include "error.hpp"

Error clean(std::string msg) {
    msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
    return msg;
}

static std::string log(const std::experimental::source_location &location) {
    std::ostringstream oss;
    oss << location.file_name() << ":" << location.line() << " `" << location.function_name() << "`";
    return oss.str();
}

Error wrap(Error error, const std::experimental::source_location &location) {
    if (error.has_value()) {
        return log(location) + ": " + error.value();
    }
    return {};
}
