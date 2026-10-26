#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

std::string shell_quote(const std::string& value) {
    std::string result = "'";
    for (char c : value) {
        if (c == '\'') {
            result += "'\\''";
        }
        else {
            result += c;
        }
    }
    return result + "'";
}

std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    size_t start = value.find_first_not_of(whitespace);

    if (start == std::string::npos) {
        return "";
    }

    size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

std::string check_git(const fs::path& path) {
    if (!fs::is_directory(path)) {
        return path.string() + " is not a directory\n";
    }

    std::string git = "git -C " + shell_quote(path.string());

    if (std::system(
            (git + " rev-parse --is-inside-work-tree >/dev/null 2>&1").c_str()
        ) != 0) {
        return "not a valid git project\n";
    }

    if (std::system((git + " remote get-url origin >/dev/null 2>&1").c_str()) !=
        0) {
        return "no origin remote configured\n";
    }

    return "";
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: " << argv[0] << " [git project path]\n";
        return EXIT_FAILURE;
    }

    fs::path path =
        fs::absolute(argc == 2 ? fs::path(argv[1]) : fs::current_path());

    std::string check = check_git(path);

    if (!check.empty()) {
        std::cout << check;
        return EXIT_FAILURE;
    }

    std::string git = "git -C " + shell_quote(path.string());

    bool needs_add = std::system((git + " diff --cached --quiet").c_str()) == 0;
    std::string input;

    while (true) {
        std::string option;
        std::string commit;
        int choice;

        std::cout << "Type of commit?\n"
                  << "1 - feat     -- New feature\n"
                  << "2 - fix      -- Bug fix\n"
                  << "3 - docs     -- Documentation change\n"
                  << "4 - style    -- Formatting, no logic changes\n"
                  << "5 - refactor -- Restructuring code, no bugs/features\n"
                  << "6 - perf     -- Performance improvement\n"
                  << "7 - test     -- Adding/fixing tests\n"
                  << "8 - build    -- Build system or dependencies\n"
                  << "9 - ci       -- CI configuration/scripts\n"
                  << "10 - chore   -- Routine maintenance, cleanups\n"
                  << "11 - revert  -- Undo a previous commit\n"
                  << "Enter type (1-11): ";

        std::getline(std::cin, option);

        if (option.empty()) {
            std::cout << "Option cannot be empty\n";
            continue;
        }

        size_t parsed = 0;

        try {
            choice = std::stoi(option, &parsed);

            if (parsed != option.size()) {
                throw std::invalid_argument("invalid option");
            }
        } catch (...) {
            std::cout << option << " is not a valid option\n";
            continue;
        }

        if (choice < 1 || choice > 11) {
            std::cout << option << " is not a valid option\n";
            continue;
        }

        switch (choice) {
            case 1:
                commit = "feat";
                break;
            case 2:
                commit = "fix";
                break;
            case 3:
                commit = "docs";
                break;
            case 4:
                commit = "style";
                break;
            case 5:
                commit = "refactor";
                break;
            case 6:
                commit = "perf";
                break;
            case 7:
                commit = "test";
                break;
            case 8:
                commit = "build";
                break;
            case 9:
                commit = "ci";
                break;
            case 10:
                commit = "chore";
                break;
            case 11:
                commit = "revert";
                break;
        }

        std::cout
            << "\nScope of this commit? (press enter to skip)\n Enter scope: ";
        std::getline(std::cin, option);

        option = trim(option);

        if (!option.empty()) {
            for (char& c : option) {
                c = static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))
                );
            }

            commit += "(" + option + ")";
        }

        commit += ": ";

        std::cout << "\nCommit description?\n Enter commit description: ";
        std::getline(std::cin, option);

        if (!option.empty()) {
            option[0] = static_cast<char>(
                std::tolower(static_cast<unsigned char>(option[0]))
            );

            if (option.back() == '.') {
                option.pop_back();
            }
        }

        commit += option;

        std::cout << "\nFinal commit message: " << commit << "\n";
        std::cout << "Start over? y/N: ";

        std::getline(std::cin, option);

        for (char& c : option) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        if (option == "y") {
            continue;
        }

        if (needs_add) {
            std::cout
                << "\nNo staged files.\n"
                << "Enter file paths separated by commas (press enter to add "
                   "everything):\n";
            std::getline(std::cin, input);
            input = trim(input);

            if (input.empty()) {
                input = ".";
            }

            std::vector<std::string> files;
            size_t start = 0;

            while (start <= input.size()) {
                size_t comma = input.find(',', start);
                std::string file = trim(input.substr(
                    start,
                    comma == std::string::npos ? std::string::npos
                                               : comma - start
                ));

                if (file.empty()) {
                    std::cout << "File path cannot be empty\n";
                    return EXIT_FAILURE;
                }

                fs::path file_path(file);
                fs::path full_path =
                    file_path.is_absolute() ? file_path : path / file_path;

                std::error_code error;

                if (!fs::exists(full_path, error) || error) {
                    std::cout << file << " does not exist\n";
                    return EXIT_FAILURE;
                }

                files.push_back(file);

                if (comma == std::string::npos) {
                    break;
                }

                start = comma + 1;
            }

            std::string add_command = git + " add --";

            for (const std::string& file : files) {
                add_command += " " + shell_quote(file);
            }

            if (std::system(add_command.c_str()) != 0) {
                std::cout << "Failed to add files\n";
                return EXIT_FAILURE;
            }
        }

        std::string command = git + " commit -m " + shell_quote(commit);

        if (std::system(command.c_str()) != 0) {
            return EXIT_FAILURE;
        }

        std::cout
            << "\nPush to what branch? (press enter to skip)\n Enter branch: ";

        std::string branch;
        std::getline(std::cin, branch);
        branch = trim(branch);

        if (branch.empty()) {
            return EXIT_SUCCESS;
        }

        std::string push_command =
            git + " push origin -- " + shell_quote(branch);

        return std::system(push_command.c_str()) == 0 ? EXIT_SUCCESS
                                                      : EXIT_FAILURE;
    }
}
