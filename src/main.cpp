#include <filesystem>
#include <system_error>

#include <map>
#include <vector>
#include <fmt/format.h>

#include "hash.hpp"

namespace fs = std::filesystem;

static void get_files_digest(std::map<std::string, std::vector<std::string>>& file_hashes, fs::recursive_directory_iterator dir_it) {
    Hasher hasher;
    for (auto&& entry: dir_it) {
        if (!entry.is_symlink() && entry.is_regular_file()) {
            auto digest = hasher.calculate_file(entry.path().generic_string().c_str());

            auto existing_hash = file_hashes.find(digest);
            if (existing_hash == file_hashes.end()) {
                std::vector<std::string> files;
                files.emplace_back(fs::absolute(entry.path()).generic_string());

                file_hashes.emplace(digest, files);
            } else {
                existing_hash->second.emplace_back(fs::absolute(entry.path()).generic_string());
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fmt::print("Usage: fs-dips-find <path1> <path2>\n");
        return 1;
    }

    std::vector<fs::recursive_directory_iterator> dirs;
    {
        std::error_code error_code;
        const fs::path first = fs::absolute(argv[1]);
        const fs::path second = fs::absolute(argv[2]);
        fs::recursive_directory_iterator first_it(first, fs::directory_options::none, error_code);

        if (error_code) {
            fmt::print("{}: Error opening: {}", argv[1], error_code.message());
            return 1;
        }

        fs::recursive_directory_iterator second_it(second, fs::directory_options::none, error_code);

        if (error_code) {
            fmt::print("{}: Error opening: {}", argv[2], error_code.message());
            return 1;
        }

        //We do not walk a directory if it is already inside another one
        //Simple comparison using abs path should give us it... hopefully
        if (first.native().find(second.native()) == fs::path::string_type::npos) {
            dirs.emplace_back(std::move(first_it));
        }
        if (second.native().find(first.native()) == fs::path::string_type::npos) {
            dirs.emplace_back(std::move(second_it));
        }

        if (dirs.empty()) {
            //Well if we're supplied with the same paths
            //then at least try to look for dups in one
            dirs.emplace_back(std::move(first_it));
        }
    }

    std::map<std::string, std::vector<std::string>> file_hashes;
    for (auto&& dir : dirs) {
        get_files_digest(file_hashes, dir);
    }

    for (auto&& [digest, files] : file_hashes) {
        if (files.size() > 1) {
            fmt::print("Found duplicate files with hash {}:\n", digest);
            for (auto&& file : files) {
                fmt::print("{}\n", file);
            }

            fmt::print("==============================\n");
        }
    }

    return 0;
}
