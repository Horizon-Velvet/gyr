#pragma once

#include <string_view>

namespace render::ansi {
inline constexpr std::string_view csi = "\x1b[";
inline constexpr std::string_view clear = "\x1b[2J";
inline constexpr std::string_view home = "\x1b[H";
inline constexpr std::string_view erase_line = "\x1b[K";
inline constexpr std::string_view erase_below = "\x1b[J";
inline constexpr std::string_view hide_cur = "\x1b[?25l";
inline constexpr std::string_view show_cur = "\x1b[?25h";
inline constexpr std::string_view screen_enable = "\x1b[?1049h";
inline constexpr std::string_view screen_disable = "\x1b[?1049l";
inline constexpr std::string_view invert = "\x1b[7m";
inline constexpr std::string_view reset = "\x1b[0m";
} // namespace render::ansi
