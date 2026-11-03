#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>

#include "exports/json.hpp"
#include "exports/picosha2.h"
#include "logs.hpp"

namespace fs = std::filesystem;
using namespace nlohmann;

namespace cmds
{
inline void init(const fs::path path, const bool is_verbose) {
    std::vector<fs::path> dirs = {
        path / ".gmgr/cache/temp",
        path / ".gmgr/cache/snapshots",
        path / ".gmgr/manifests",
    };

    std::vector<fs::path> files = {path / "about.jsonc",
        path / ".gmgr/manifests/about.manifest.sha256",
        path / ".gmgr/manifests/saved.manifest.sha256",
        path / ".gmgr/manifests/snapshots.manifest.sha256"};

    json about_content_obj = {{"project",
                                  {{"name", ""},
                                      {"description", ""},
                                      {"version", {0, 1, 0}},
                                      {"settings", json::object()}}},
        {"snapshots", json::array()}};

    std::string about_content = about_content_obj.dump();

    std::string about_sha;
    std::string saved_sha;
    std::string snapshots_sha;

    logs::log_info("Initializing project");

    logs::log_debug("Creating directories", is_verbose);

    for (const fs::path& dir : dirs) {
        logs::log_debug(std::format("Creating {}", fs::relative(dir).string()),
            is_verbose);

        fs::create_directories(dir);
    }

    logs::log_debug("Finished creating directories", is_verbose);
    logs::log_debug("Creating files", is_verbose);

    for (const fs::path& file_name : files) {
        logs::log_debug(std::format("Creating {}", fs::relative(file_name).string()),
            is_verbose);

        std::ofstream file(file_name);

        if (file_name == files[0]) {
            logs::log_debug(std::format("Writing {}", std::string(files[0])), is_verbose);
            if (file.is_open()) {
                json about = json::parse(about_content_obj.dump(4), nullptr, true, true);
                file << about_content_obj.dump(4);
            }
            else {
                logs::log_error(std::format("Cannot open {}", std::string(files[0])));
            }
        }
        else if (file_name == files[1]) {
            logs::log_debug(std::format("Writing checksum to {}", std::string(files[1])));
            if (file.is_open()) {
                picosha2::hash256_hex_string(about_content, about_sha);
                logs::log_debug(std::format("Generated sha256 checksum: {}", about_sha));
                file << about_sha;
            }
            else {
                logs::log_error(std::format("Cannot open {}", std::string(files[1])));
            }
        }
    }
    logs::log_debug("Finished creating files", is_verbose);

    logs::log_info(std::format("Finished creating {}", path.string()));
}

// inline void check(const fs::path path, const bool is_verbose) {
//     logs::log_info("Checking project");
//     logs::log_info("Checking .gmgr/");
// }
} // namespace cmds