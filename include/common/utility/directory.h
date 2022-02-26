
#ifndef SANDBOX_DIRECTORY_H
#define SANDBOX_DIRECTORY_H

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

    // Could be file or directory.
    [[nodiscard]] bool Exists(const std::string& in);

    // Passing in a filepath will create all the directories leading up to the file location.
    void CreateDirectory(const std::string& in);
    void CreateFile(const std::string& in);

    [[nodiscard]] std::string GetWorkingDirectory();

    // Returns the path (relative to system root) of given directory / file.
    // Note: assumes 'in' is contained within the scope of the project.
    [[nodiscard]] std::string GetGlobalPath(const std::string& in);

}

#endif //SANDBOX_DIRECTORY_H
