
#ifndef SANDBOX_DIRECTORY_UTILS_H
#define SANDBOX_DIRECTORY_UTILS_H

#include "pch.h"

namespace Sandbox {

    [[nodiscard]] std::string ToLower(const std::string& in);

    [[nodiscard]] std::string ConvertToNativeSeparators(const std::string& in);
    [[nodiscard]] char GetNativeSeparator();

    // Returns ONLY the asset name.
    [[nodiscard]] std::string GetAssetName(const std::string& file);

    // Returns ONLY the asset extension.
    [[nodiscard]] std::string GetAssetExtension(const std::string& file);

    // Returns asset directory with no leading separator.
    [[nodiscard]] std::string GetAssetDirectory(const std::string& file);

    [[nodiscard]] std::vector<std::string> GetFiles(const std::string& directory);

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
