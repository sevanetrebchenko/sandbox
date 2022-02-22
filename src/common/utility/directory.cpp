
#include "common/utility/directory.h"

namespace Sandbox {

    std::string ToLower(const std::string& in) {
        std::string out = in;
        for (char& character : out) {
            character = static_cast<char>(std::tolower(character));
        }
        return out;
    }

    std::string ConvertToNativeSeparators(const std::string& in) {
        std::string out = in;

    #ifdef _WIN32
        const char separator = '/';
        const char native = '\\';
    #else
        const char separator = '\\';
        const char native = '/';
    #endif

        std::replace(out.begin(), out.end(), separator, native);
        return out;
    }

    char GetNativeSeparator() {
    #ifdef _WIN32
        return '\\';
    #else
        return '/';
    #endif
    }

    std::string GetAssetName(const std::string& file) {
        std::string path = ConvertToNativeSeparators(file);
        std::size_t slashPosition;

    #ifdef _WIN32
        slashPosition = path.find_last_of('\\');
    #else
        slashPosition = path.find_last_of('/');
    #endif

        // If there exists a slash.
        if (slashPosition != std::string::npos) {
            path = path.substr(slashPosition + 1);

            std::size_t dotPosition = path.find_first_of('.');

            if (dotPosition != std::string::npos) {
                return path.substr(0, dotPosition);
            }
        }
        return path;
    }

    std::string GetAssetExtension(const std::string& file) {
        std::size_t position = file.find_last_of('.');

        if (position == std::string::npos) {
            // No character found.
            return { "" };
        }
        else {
            // Don't include '.'
            // TODO: Case sensitive?
            return file.substr(position + 1);
        }
    }

    std::string GetAssetDirectory(const std::string& file) {
        std::string directory = ConvertToNativeSeparators(file);
        std::size_t slashPosition = directory.find_last_of(GetNativeSeparator());

        if (slashPosition == std::string::npos) {
            return { "" };
        }
        else {
            return directory.substr(0, slashPosition);
        }
    }

    std::vector<std::string> GetFiles(const std::string& directory) {
        std::filesystem::path path { ConvertToNativeSeparators(directory) };
        if (!std::filesystem::is_directory(path)) {
            throw std::runtime_error("Provided directory does not exist.");
        }

        std::vector<std::string> files;

        for (auto& file : std::filesystem::directory_iterator(path)) {
            files.push_back(file.path().string());
        }

        return files;
    }

}
