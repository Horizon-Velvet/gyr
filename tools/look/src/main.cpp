#include <csignal>
#include <filesystem>
#include <ios>
#include <iostream>
#include <system_error>

#include "../inc/render.hpp"

namespace fs = std::filesystem;

volatile std::sig_atomic_t g_running = 1;

void handle_sigint(int) { g_running = 0; }

int main(int argc, char *argv[]) {
    if (argc > 2) {
        return 1;
    }

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::error_code ec;
    auto state = render::SharedState();
    state.cwd = fs::current_path(ec);
    if (ec) {
        std::cout << ec.message();
        return 1;
    }

    std::signal(SIGINT, handle_sigint);

    render::Screen screen;

    if (argc == 2 && fs::exists(argv[1]) && fs::is_directory(argv[1])) {
        state.cwd = argv[1];
    }

    render::reload(state);

    while (state.running && g_running) {
        render::reload(state);

        char ch;
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            if (ch == 'q') {
                state.running = false;
            }
        }
    }

    return 0;
}
