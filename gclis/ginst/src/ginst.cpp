
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <system_error>
#include <unistd.h>

#include "../inc/exports/CLI11.hpp"
#include "../inc/logs.hpp"

namespace fs = std::filesystem;

fs::path get_fallback_path() {
    const char* home = std::getenv("HOME");
    return fs::path(home != nullptr ? home : "/root") / ".local" / "bin";
}

fs::path resolve_file_path(const fs::path& file_path) {
    std::error_code ec;

    const fs::path local_path = fs::absolute(file_path, ec);

    if (!ec && fs::is_regular_file(local_path, ec)) {
        return local_path;
    }

    if (file_path.has_parent_path()) {
        return {};
    }

    const char* path_env = std::getenv("PATH");

    if (path_env == nullptr) {
        return {};
    }

    const std::string paths = path_env;
    std::size_t start = 0;

    while (start <= paths.size()) {
        const std::size_t end = paths.find(':', start);

        const std::string entry = paths.substr(start,
            end == std::string::npos ? std::string::npos : end - start);

        const fs::path directory = entry.empty() ? fs::current_path() : fs::path(entry);

        const fs::path candidate = directory / file_path;

        if (fs::is_regular_file(candidate, ec)) {
            return candidate;
        }

        if (end == std::string::npos) {
            break;
        }

        start = end + 1;
    }

    return {};
}

void install_file(const fs::path& file_path,
    const fs::path& default_path,
    const fs::path& fallback_path) {
    try {
        const fs::path source = resolve_file_path(file_path);

        if (source.empty()) {
            logs::log_info(std::format("{} is not a valid file", file_path.string()));
            return;
        }

        const fs::path install_dir =
            fs::is_directory(default_path) ? default_path : fallback_path;

        fs::create_directories(install_dir);

        const fs::path destination = install_dir / source.filename();

        std::error_code ec;

        if (fs::equivalent(source, destination, ec) && !ec) {
            logs::log_info(std::format("{} is already installed", destination.string()));
            return;
        }

        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);

        const fs::perms exec_bits =
            fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec;

        fs::permissions(destination, exec_bits, fs::perm_options::add);

        logs::log_info(std::format("Installed {}", destination.string()));
    } catch (const fs::filesystem_error& e) {
        logs::log_error(std::format("Installation failed: {}", e.what()));
    }
}

bool remove_installed_file(const fs::path& directory, const std::string& file_name) {
    const fs::path file_path = directory / file_name;

    return fs::is_regular_file(file_path) && fs::remove(file_path);
}

void uninstall_file(const std::string& file_name,
    const fs::path& default_path,
    const fs::path& fallback_path,
    bool force) {
    const fs::path name(file_name);

    if (name.empty() || name.has_parent_path() || name == "." || name == "..") {
        logs::log_warn("Please provide a valid file name.");
        return;
    }

    try {
        const bool found = fs::is_regular_file(default_path / name) ||
                           fs::is_regular_file(fallback_path / name);

        if (!found) {
            logs::log_info(std::format("{} is not installed", file_name));
            return;
        }

        if (!force) {
            std::cout << std::format("Uninstall {} (y/N)? ", file_name) << std::flush;

            std::string answer;
            std::getline(std::cin, answer);

            if (answer != "y" && answer != "Y") {
                logs::log_info("Uninstallation cancelled.");
                return;
            }
        }

        const bool removed_default = remove_installed_file(default_path, file_name);

        const bool removed_fallback = remove_installed_file(fallback_path, file_name);

        if (removed_default || removed_fallback) {
            logs::log_info(std::format("Uninstalled {}", file_name));
        }
    } catch (const fs::filesystem_error& e) {
        logs::log_error(std::format("Uninstallation failed: {}", e.what()));
    }
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const fs::path default_path = "/usr/local/bin";
    const fs::path fallback_path = get_fallback_path();

    CLI::App app("A simple installer.", "ginst");

    app.set_version_flag("-V, --version",
        "ginst (gyr) v0.1.0-alpha\n\nLicensed under the Apache 2.0 License");

    app.fallthrough();

    bool verbose = false;

    app.add_flag("-v, --verbose",
        verbose,
        "Increases extra information when running a command.");

    fs::path file_path;

    auto install_cmd = app.add_subcommand("install",
        "Installs an executable or updates an existing installation.");

    install_cmd->add_option("file_path", file_path, "The path to install the executable.")
        ->required();

    install_cmd->callback([&] {
        install_file(file_path, default_path, fallback_path);
    });

    std::string file_name;
    bool force = false;

    auto uninstall_cmd = app.add_subcommand("uninstall", "Uninstalls an executable.");

    uninstall_cmd
        ->add_option("file_name", file_name, "The name of the executable to uninstall.")
        ->required();

    uninstall_cmd->add_flag("-f, --force", force, "Skip uninstall confirmation.");

    uninstall_cmd->callback([&] {
        uninstall_file(file_name, default_path, fallback_path, force);
    });

    CLI11_PARSE(app, argc, argv);

    return 0;
}
