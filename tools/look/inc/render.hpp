#pragma once

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

#include "ansi.hpp"

namespace fs = std::filesystem;

namespace render {
class Screen {
  public:
    Screen() {
        if (tcgetattr(STDIN_FILENO, &orig_termios_) == -1) {
            throw std::runtime_error("tcgetattr failed");
        }

        termios raw = orig_termios_;
        cfmakeraw(&raw);

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
            throw std::runtime_error("tcsetattr failed");
        }

        active_ = true;

        std::cout << ansi::esc::enable_buf << ansi::esc::hide_cursor
                  << std::flush;
    }

    ~Screen() { restore(); }

    Screen(const Screen &) = delete;
    Screen &operator=(const Screen &) = delete;

  private:
    termios orig_termios_{};
    bool active_ = false;

    void restore() noexcept {
        if (active_) {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios_);
            active_ = false;
        }

        std::cout << ansi::esc::disable_buf << ansi::esc::show_cursor
                  << std::flush;
    }
};
struct SharedState {
    fs::path cwd;
    std::size_t selected = 0;
    std::string input;
    bool running = true;
};

void reload(const SharedState &state);
} // namespace render
