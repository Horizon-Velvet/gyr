#include <filesystem>
#include <ostream>
#include <system_error>
#include <vector>

#include "../inc/ansi.hpp"
#include "../inc/render.hpp"

namespace fs = std::filesystem;

using namespace ansi;

namespace render {
void reload(const SharedState &state) {
    std::cout << csi::home << csi::erase_after << state.cwd.string()
              << csi::beg_down(1);

    std::error_code ec;
    std::vector<fs::directory_entry> entries;

    for (const auto &entry : fs::directory_iterator(state.cwd, ec)) {
        entries.push_back(entry);
    }

    if (ec) {
        std::cerr << ec.message();
        return;
    }

    std::cout << csi::erase_ln_all << (entries.empty() ? "└── " : "├── ")
              << c16::h_blue << ".." << gfx::reset << '/' << csi::beg_down(1);

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const auto &entry = entries[i];
        const bool is_last = (i == entries.size() - 1);
        const char *branch = is_last ? "└── " : "├── ";

        std::error_code entry_ec;

        const bool is_sym = entry.is_symlink(entry_ec);
        const bool is_dir = entry.is_directory(entry_ec);
        const auto name = entry.path().filename().string();

        std::cout << csi::erase_ln_all << branch;

        if (is_sym && is_dir) {
            std::cout << c16::h_blue << name << gfx::reset << "@/";
        } else if (is_sym) {
            std::cout << name << gfx::reset << '@';
        } else if (is_dir) {
            std::cout << c16::h_blue << name << gfx::reset << '/';
        } else {
            std::cout << name;
        }

        std::cout << csi::beg_down(1);
    }

    std::cout << std::flush;
}

} // namespace render
