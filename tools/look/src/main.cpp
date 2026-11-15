#include <filesystem>
#include <ios>
#include <iostream>
#include <system_error>

#include "../inc/render.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;

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
    if (argc == 2 && fs::exists(argv[1]) && fs::is_directory(argv[1])) {
        state.cwd = argv[1];
    }

    // main

    render::Screen screen;

    while (state.running) {
        render::reload(state);

        char ch;
        if (read(STDIN_FILENO, &ch, 1) != 1) {
            continue;
        }

        switch (ch) {
        case 'q':
            state.running = false;
            break;
        }
    }

    return 0;
}
