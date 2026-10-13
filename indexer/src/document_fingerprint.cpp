#include "document_fingerprint.h"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <functional>

std::size_t calculateDocumentFingerprint(const std::string& directoryPath) {
    namespace fs = std::filesystem;

    std::vector<fs::path> files;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());

    std::size_t fingerprint = 0;

    for (const auto& path : files) {
        std::string name = path.filename().string();
        auto size = fs::file_size(path);
        auto modified = fs::last_write_time(path).time_since_epoch().count();

        auto combine = [&fingerprint](std::size_t value) {
            fingerprint ^= value + 0x9e3779b9
                         + (fingerprint << 6)
                         + (fingerprint >> 2);
        };

        combine(std::hash<std::string>{}(name));
        combine(std::hash<std::uintmax_t>{}(size));
        combine(std::hash<decltype(modified)>{}(modified));
    }

    return fingerprint;
}