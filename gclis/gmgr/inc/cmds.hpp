#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>

#include "exports/json.hpp"
#include "logs.hpp"

namespace fs = std::filesystem;
using namespace nlohmann;

namespace cmds
{

inline void init(const fs::path path, const bool is_verbose) {
    std::vector<fs::path> dirs = {
        path / ".gnit/cache/temp",
        path / ".gnit/cache/snapshots",
    };

    std::vector<fs::path> files = {path / "about.jsonc"};

    std::string about_content = R"({
    "project": {
        "name": "",
        "description": "",
        "version": [
            0,
            1,
            0
            // optional pre-release tags
        ],
        "settings": {}
    },
    // don't edit manually
    "snapshots": [],
    // supports regular expressions
    "excludes": []
}
)";

    logs::log_info("Initializing project");

    logs::log_debug("Creating directories", is_verbose);

    for (const fs::path& dir : dirs) {
        logs::log_debug(std::format("Creating {}", fs::relative(dir).string()),
            is_verbose);

        fs::create_directories(dir);
    }

    logs::log_debug("Creating files", is_verbose);

    for (const fs::path& file_name : files) {
        logs::log_debug(std::format("Creating {}", fs::relative(file_name).string()),
            is_verbose);

        std::ofstream file(file_name);

        if (file_name == files[0]) {
            json about = json::parse(about_content, nullptr, true, true);
            file << about_content;
        }
    }

    logs::log_info(std::format("Finished creating {}", path.string()));
}

} // namespace cmds