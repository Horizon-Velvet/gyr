#ifndef GYR_ANSI
#define GYR_ANSI

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <format>
#include <iostream>
#include <poll.h>
#include <string>
#include <string_view>
#include <unistd.h>
#include <utility>

namespace ansi {

namespace gfx {
inline constexpr std::string_view reset = "\x1B[0m";
inline constexpr std::string_view rst_bold = "\x1B[22m";
inline constexpr std::string_view rst_dim = "\x1B[22m";
inline constexpr std::string_view rst_italic = "\x1B[23m";
inline constexpr std::string_view rst_underln = "\x1B[24m";
inline constexpr std::string_view rst_blink = "\x1B[25m";
inline constexpr std::string_view rst_inverse = "\x1B[27m";
inline constexpr std::string_view rst_hidden = "\x1B[28m";
inline constexpr std::string_view rst_strike = "\x1B[29m";

inline constexpr std::string_view bold = "\x1B[1m";
inline constexpr std::string_view dim = "\x1B[2m";
inline constexpr std::string_view italic = "\x1B[3m";
inline constexpr std::string_view underln = "\x1B[4m";
inline constexpr std::string_view blink = "\x1B[5m";
inline constexpr std::string_view inverse = "\x1B[7m";
inline constexpr std::string_view hidden = "\x1B[8m";
inline constexpr std::string_view strike = "\x1B[9m";
} // namespace gfx

namespace c16 {
inline constexpr std::string_view black = "\x1B[30m";
inline constexpr std::string_view red = "\x1B[31m";
inline constexpr std::string_view green = "\x1B[32m";
inline constexpr std::string_view yellow = "\x1B[33m";
inline constexpr std::string_view blue = "\x1B[34m";
inline constexpr std::string_view magenta = "\x1B[35m";
inline constexpr std::string_view cyan = "\x1B[36m";
inline constexpr std::string_view white = "\x1B[37m";

inline constexpr std::string_view black_bkg = "\x1B[40m";
inline constexpr std::string_view red_bkg = "\x1B[41m";
inline constexpr std::string_view green_bkg = "\x1B[42m";
inline constexpr std::string_view yellow_bkg = "\x1B[43m";
inline constexpr std::string_view blue_bkg = "\x1B[44m";
inline constexpr std::string_view magenta_bkg = "\x1B[45m";
inline constexpr std::string_view cyan_bkg = "\x1B[46m";
inline constexpr std::string_view white_bkg = "\x1B[47m";

inline constexpr std::string_view h_black = "\x1B[90m";
inline constexpr std::string_view h_red = "\x1B[91m";
inline constexpr std::string_view h_green = "\x1B[92m";
inline constexpr std::string_view h_yellow = "\x1B[93m";
inline constexpr std::string_view h_blue = "\x1B[94m";
inline constexpr std::string_view h_magenta = "\x1B[95m";
inline constexpr std::string_view h_cyan = "\x1B[96m";
inline constexpr std::string_view h_white = "\x1B[97m";

inline constexpr std::string_view h_black_bkg = "\x1B[100m";
inline constexpr std::string_view h_red_bkg = "\x1B[101m";
inline constexpr std::string_view h_green_bkg = "\x1B[102m";
inline constexpr std::string_view h_yellow_bkg = "\x1B[103m";
inline constexpr std::string_view h_blue_bkg = "\x1B[104m";
inline constexpr std::string_view h_magenta_bkg = "\x1B[105m";
inline constexpr std::string_view h_cyan_bkg = "\x1B[106m";
inline constexpr std::string_view h_white_bkg = "\x1B[107m";
} // namespace c16

namespace c256 {
inline constexpr std::string rgb(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);

    int index = 16 + 36 * ((r * 5 + 127) / 255) + 6 * ((g * 5 + 127) / 255) +
                ((b * 5 + 127) / 255);

    return std::format("\033[38;5;{}m", index);
}
} // namespace c256

namespace csi {
inline constexpr std::string_view home = "\x1B[H";
inline constexpr std::string_view pos_raw = "\x1B[6n";

inline constexpr std::string_view erase_after = "\x1B[0J";
inline constexpr std::string_view erase_before = "\x1B[1J";
inline constexpr std::string_view erase_all = "\x1B[2J";
inline constexpr std::string_view erase_old = "\x1B[3J";

inline constexpr std::string_view erase_ln_after = "\x1B[0K";
inline constexpr std::string_view erase_ln_before = "\x1B[1K";
inline constexpr std::string_view erase_ln_all = "\x1B[2K";

inline std::string set(unsigned int r, unsigned int c) {
    return std::format("\x1B[{};{}H", r, c);
}

inline std::string up(unsigned int n) { return std::format("\x1B[{}A", n); }

inline std::string down(unsigned int n) { return std::format("\x1B[{}B", n); }

inline std::string right(unsigned int n) { return std::format("\x1B[{}C", n); }

inline std::string left(unsigned int n) { return std::format("\x1B[{}D", n); }

inline std::string beg_down(unsigned int n) {
    return std::format("\x1B[{}E", n);
}

inline std::string beg_up(unsigned int n) { return std::format("\x1B[{}F", n); }

inline std::string scroll_down(unsigned int n) {
    return std::format("\x1B[{}S", n);
}
inline std::string scroll_up(unsigned int n) {
    return std::format("\x1B[{}T", n);
}

inline std::pair<unsigned int, unsigned int> pos_norm() {
    char buf[32]{};
    std::cout << pos_raw << std::flush;

    std::size_t len = 0;

    while (len < sizeof(buf) - 1) {
        pollfd pfd{STDIN_FILENO, POLLIN, 0};

        int ready;
        do {
            ready = poll(&pfd, 1, 1000);
        } while (ready < 0 && errno == EINTR);

        if (ready <= 0 || !(pfd.revents & POLLIN)) {
            return {0, 0};
        }

        char ch{};
        ssize_t n;

        do {
            n = read(STDIN_FILENO, &ch, 1);
        } while (n < 0 && errno == EINTR);

        if (n != 1) {
            return {0, 0};
        }

        buf[len++] = ch;

        if (ch == 'R') {
            buf[len] = '\0';

            unsigned int row{}, col{};

            if (std::sscanf(buf, "\x1B[%u;%uR", &row, &col) != 2) {
                return {0, 0};
            }

            return {row, col};
        }
    }

    return {0, 0};
}
} // namespace csi

namespace esc {
inline constexpr std::string_view save = "\x1B"
                                         "7";
inline constexpr std::string_view restore = "\x1B"
                                            "8";
inline constexpr std::string_view index = "\x1B"
                                          "D";
inline constexpr std::string_view reverse_index = "\x1B"
                                                  "M";

inline constexpr std::string_view hide_cursor = "\x1B[?25l";
inline constexpr std::string_view show_cursor = "\x1B[?25h";

inline constexpr std::string_view enable_buf = "\x1B[?1049h";
inline constexpr std::string_view disable_buf = "\x1B[?1049l";
} // namespace esc

} // namespace ansi

#endif