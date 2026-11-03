#pragma once

#include <iostream>
#include <string_view>

namespace logs
{
inline void log_info(std::string_view message) {
    std::cout << "[info] " << message << '\n';
}

inline void log_error(std::string_view message) {
    std::cerr << "[\033[1mERROR\033[22m] " << message << '\n';
}

inline void log_debug(std::string_view message) {
    std::cout << "[debug] " << message << '\n';
}

inline void log_debug(std::string_view message, bool run) {
    if (!run) {
        return;
    }
    std::cout << "[debug] " << message << '\n';
}
} // namespace log
