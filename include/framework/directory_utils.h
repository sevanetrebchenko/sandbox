
#ifndef SANDBOX_DIRECTORY_UTILS_H
#define SANDBOX_DIRECTORY_UTILS_H

#include <sandbox_pch.h>

namespace Sandbox {

    class DirectoryReader {
        public:
            [[nodiscard]] static std::vector<std::string> GetFiles(std::string directoryPath);

            [[nodiscard]] static std::vector<std::string> GetAssetNames(const std::vector<std::string>& filePaths);
            [[nodiscard]] static std::string GetAssetName(const std::string& assetPath);

            [[nodiscard]] static std::string GetAssetExtension(const std::string& assetPath);
    };

    class NativePathConverter {
        public:
            [[nodiscard]] static std::string ConvertToNativeSeparators(std::string path);
    };

    class IReloadable {
        public:
            IReloadable(const std::initializer_list<std::string>& trackingFiles);
            ~IReloadable();

            void RecompileIfModified();

        private:
            virtual void OnFileModified() = 0;
            std::vector<std::string> _filePaths;
            std::vector<std::filesystem::file_time_type> _fileModifyTimes;
    };

}

#endif //SANDBOX_DIRECTORY_UTILS_H
