
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
        std::size_t slashPosition = path.find_last_of(GetNativeSeparator());

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

    std::string GetAssetExtension(const std::string& in) {
        std::string file = ConvertToNativeSeparators(in);

        std::size_t separator = file.find_last_of(GetNativeSeparator());
        std::size_t dot = file.find_last_of('.');

        if (separator == std::string::npos) {
            // Filepath has no leading directories.
            if (dot == std::string::npos) {
                // Extension is blank.
                return { "" };
            }
            else {
                return file.substr(dot + 1);
            }
        }
        else {
            if (dot < separator) {
                // Position of the dot comes before the last separator.
                // Case where one of the directories in the path has a dot in it.
                // Extension is blank.
                return { "" };
            }
            else {
                return file.substr(dot + 1);
            }
        }
    }

    std::string GetAssetDirectory(const std::string& in) {
        std::string file = ConvertToNativeSeparators(in);
        std::string extension = GetAssetExtension(file);

        if (extension.empty()) {
            throw std::runtime_error("Path provided to GetAssetDirectory is not a file.");
        }

        if (!Exists(file)) {
            throw std::runtime_error("File provided to GetAssetDirectory does not exist.");
        }

        return std::filesystem::path(file).parent_path().string();
    }

    std::vector<std::string> GetFiles(const std::string& in) {
        std::string directory = ConvertToNativeSeparators(in);
        std::string extension = GetAssetExtension(directory);

        if (!extension.empty()) {
            throw std::runtime_error("Path provided to GetFiles is not a directory.");
        }

        if (!Exists(directory)) {
            throw std::runtime_error("Directory provided to GetFiles does not exist.");
        }

        std::vector<std::string> files;

        for (auto& file : std::filesystem::directory_iterator(std::filesystem::path(directory))) {
            files.push_back(file.path().string());
        }

        return files;
    }

    bool Exists(const std::string& in) {
        return std::filesystem::exists({ ConvertToNativeSeparators(in) });
    }

    void CreateDirectory(const std::string& in) {
        std::filesystem::create_directories(ConvertToNativeSeparators(in));
    }

    void CreateFile(const std::string& in) {
        std::string filepath = ConvertToNativeSeparators(in);
        std::string extension = GetAssetExtension(filepath);

        if (extension.empty()) {
            throw std::runtime_error("Directory passed into CreateFile. Use CreateDirectory instead.");
        }

        // Create required directories.
        std::filesystem::path path = std::filesystem::path(filepath).parent_path();
        std::filesystem::create_directories(path);

        // Destructor closes stream.
        std::ofstream(filepath, std::ios::app); // Open for append to not erase any existing contents.
    }

    std::string GetWorkingDirectory() {
        return ConvertToNativeSeparators(std::filesystem::current_path().string());
    }

    std::string GetGlobalPath(const std::string& in) {
        return ConvertToNativeSeparators(GetWorkingDirectory() + GetNativeSeparator() + in);
    }

    std::string ProcessName(const std::string& in) {
        // Convert name to all lowercase, with underscores instead of spaces.
        std::vector<char> disallowedCharacters = { '/', '\\', ':', '*', '?', '"', '<', '>', '|' }; // Windows.

        std::stringstream builder;
        for (char character : in) {
            if (character == ' ') {
                builder << '_';
            }
            else {
                bool skipCharacter = false;

                for (char c : disallowedCharacters) {
                    if (character == c) {
                        skipCharacter = true;
                        break;
                    }
                }

                if (skipCharacter) {
                    continue;
                }

                builder << static_cast<char>(std::tolower(character));
            }
        }

        return builder.str();
    }

}
