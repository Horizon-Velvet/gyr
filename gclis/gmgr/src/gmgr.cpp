#include <filesystem>
#include <ios>
#include <iostream>

#include "../inc/cmds.hpp"
#include "../inc/exports/CLI11.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // setup
    CLI::App app("A project manager.", "gmgr");
    app.set_version_flag("-V,--version",
        "gmgr (gCLI) v0.1.1-alpha\n\nLicensed under the Apache 2.0 License");
    app.fallthrough();

    // params
    fs::path file_path;
    bool verbose = false;

    // global flags
    app.add_flag("-v, --verbose",
        verbose,
        "Increases extra information when running a command.");

    // subcommands
    const auto init_cmd = app.add_subcommand("init", "Initializes a default project.");
    init_cmd->add_option("file_path", file_path, "The path to initialize a project.")
        ->default_val(".");

    // const auto check_cmd = app.add_subcommand("save", "Saves a project.");
    // check_cmd->add_option("file_path", file_path, "The path to save a project.")
    //     ->default_val(".");

    // check_cmd->callback([&] {
    //     cmds::check(file_path, verbose);
    // });

    init_cmd->callback([&] {
        cmds::init(file_path, verbose);
    });

    CLI11_PARSE(app, argc, argv);

    return 0;
}
