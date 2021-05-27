#pragma once

#include <experimental/source_location>
#include <optional>
#include <string>

using Error = std::optional<std::string>;

Error clean(std::string msg);
Error wrap(
    Error error, const std::experimental::source_location &location = std::experimental::source_location::current());
