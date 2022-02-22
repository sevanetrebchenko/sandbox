
#include "common/utility/reloadable.h"
#include "common/utility/directory.h"

namespace Sandbox {

    IReloadable::IReloadable(const std::initializer_list<std::string>& files) {
        for (const std::string& file : files) {
            files_.emplace_back(ConvertToNativeSeparators(file));
        }

        modifyTimes_.resize(files_.size());
        for (const std::string& file : files_) {
            modifyTimes_.emplace_back(std::filesystem::last_write_time(file));
        }
    }

    IReloadable::IReloadable(const std::string& file) {
        files_.emplace_back(ConvertToNativeSeparators(file));
        modifyTimes_.emplace_back(std::filesystem::last_write_time(file));
    }

    IReloadable::~IReloadable() = default;

    bool IReloadable::IsModified() const {
        for (int i = 0; i < files_.size(); ++i) {
            if (std::filesystem::last_write_time(files_[i]) != modifyTimes_[i]) {
                return true;
            }
        }

        return false;
    }

    void IReloadable::Clear() {
        for (int i = 0; i < files_.size(); ++i) {
            modifyTimes_[i] = std::filesystem::last_write_time(files_[i]);
        }
    }

}
