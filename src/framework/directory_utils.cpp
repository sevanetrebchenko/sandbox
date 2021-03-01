
#include <framework/directory_utils.h>

namespace Sandbox {
    std::vector<std::string> DirectoryReader::GetFiles(std::string directoryPath) {
        directoryPath = NativePathConverter::ConvertToNativeSeparators(directoryPath);

        std::filesystem::path path { directoryPath };
        if (std::filesystem::is_directory(path)) {
            std::vector<std::string> filesInDirectory;

            for (auto& file : std::filesystem::directory_iterator(path)) {
                filesInDirectory.push_back(file.path().string());
            }

            return filesInDirectory;
        }

        throw std::runtime_error("Provided directory does not exist.");
    }

    std::vector<std::string> DirectoryReader::GetAssetNames(const std::vector<std::string> &filePaths) {
        std::vector<std::string> assets;

        assets.reserve(filePaths.size());
        for (auto& filePath : filePaths) {
            assets.emplace_back(GetAssetName(filePath));
        }

        return assets;
    }

    std::string DirectoryReader::GetAssetName(const std::string& assetPath) {
        std::string nativeAssetPath = NativePathConverter::ConvertToNativeSeparators(assetPath);
        std::size_t slashPosition;

        #ifdef _WIN32
            slashPosition = nativeAssetPath.find_last_of('\\');
        #else
            slashPosition = nativeAssetPath.find_last_of('/');
        #endif

        // If there exists a slash.
        if (slashPosition != std::string::npos) {
            return nativeAssetPath.substr(slashPosition + 1);
        }

        return nativeAssetPath;
    }

    std::string DirectoryReader::GetAssetExtension(const std::string &assetPath) {
        return assetPath.substr(assetPath.find_last_of('.') + 1);
    }

    std::string NativePathConverter::ConvertToNativeSeparators(std::string path) {
        #ifdef _WIN32
            const char separator = '/';
            const char native = '\\';
        #else
            const char separator = '\\';
            const char native = '/';
        #endif
        std::replace(path.begin(), path.end(), separator, native);
        return path;
    }

    IReloadable::IReloadable(const std::initializer_list<std::string> &trackingFiles) : _filePaths(trackingFiles) {
        for (const std::string& filePath : _filePaths) {
            _fileModifyTimes.emplace_back(std::filesystem::last_write_time(filePath)); // Emplace initial write time at the start of the program.
        }
    }

    IReloadable::~IReloadable() {
        _filePaths.clear();
        _fileModifyTimes.clear();
    }

    void IReloadable::RecompileIfModified() {
        bool changed = false;

        for (int i = 0; i < _filePaths.size(); ++i) {
            const std::string& filePath = _filePaths[i];
            const std::filesystem::file_time_type& fileModifyTime = _fileModifyTimes[i];

            std::filesystem::file_time_type updatedModifyTime = std::filesystem::last_write_time(filePath);

            if (fileModifyTime != updatedModifyTime) {
                changed = true;
                _fileModifyTimes[i] = updatedModifyTime;
            }
        }

        if (changed) {
            OnFileModified();
        }
    }
}
