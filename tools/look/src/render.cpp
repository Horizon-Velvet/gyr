#include <iostream>

#include "../inc/defs.hpp"
#include "../inc/render.hpp"

namespace render {
void reload(SharedState &state) {
    std::cout << render::ansi::clear << render::ansi::home << state.cwd.string()
              << std::flush;
}
} // namespace render
