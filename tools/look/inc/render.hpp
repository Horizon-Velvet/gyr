#pragma once

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

#include "defs.hpp"

namespace render {
class Screen {
  public:
    Screen() {
        if (tcgetattr(STDIN_FILENO, &orig_termios_) == -1) {
            throw std::runtime_error("tcgetattr failed");
        }

        termios raw = orig_termios_;
        cfmakeraw(&raw);
        raw.c_lflag |= ISIG;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
            throw std::runtime_error("tcsetattr failed");
        }

        active_ = true;

        std::cout << render::ansi::screen_enable << std::flush;
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

        std::cout << render::ansi::screen_disable << std::flush;
    }
};

struct SharedState {
    std::filesystem::path cwd;
    std::size_t selected;
    std::string input;
    bool running = true;
};

void reload(SharedState &state);
} // namespace render
