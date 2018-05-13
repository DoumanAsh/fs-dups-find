#include <filesystem>
#include <system_error>

#include <map>
#include <vector>
#include <fmt/format.h>

#include "hash.hpp"

namespace fs = std::filesystem;

void get_files_digest(std::map<std::string, std::vector<std::string>>& file_hashes, fs::recursive_directory_iterator dir_it) {
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

int main(int argc, char* argv[])
{
    if (argc < 3) {
        fmt::print("Usage: fs-dips-find <path1> <path2>\n");
        return 1;
    }

    std::error_code error_code;
    const fs::path first(argv[1]);
    fs::recursive_directory_iterator first_it(first, fs::directory_options::none, error_code);

    if (error_code) {
        fmt::print("{}: Error opening: {}\n", first.generic_string(), error_code.message());
        return 1;
    }

    const fs::path second(argv[2]);
    fs::recursive_directory_iterator second_it(second, fs::directory_options::none, error_code);

    if (error_code) {
        fmt::print("{}: Error opening: {}\n", second.generic_string(), error_code.message());
        return 1;
    }

    std::map<std::string, std::vector<std::string>> file_hashes;
    get_files_digest(file_hashes, first_it);
    get_files_digest(file_hashes, second_it);

    for (const auto& [digest, files] : file_hashes) {
        if (files.size() > 1) {
            fmt::print("Found duplicate files with hash {}:\n", digest);
            for (const auto& file : files) {
                fmt::print("{}\n", file);
            }

            fmt::print("==============================\n");
        }
    }

    return 0;
}
