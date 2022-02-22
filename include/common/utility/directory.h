
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

}

#endif //SANDBOX_DIRECTORY_H
