
#include "common/utility/reloadable.h"
#include "common/utility/directory.h"

namespace Sandbox {

    IReloadable::IReloadable(const std::string& file) {
        files_.emplace_back(ConvertToNativeSeparators(file));
        editTimes_.emplace_back(std::filesystem::last_write_time(file));
    }

    IReloadable::IReloadable(const std::initializer_list<std::string>& files) {
        for (const std::string& file : files) {
            files_.emplace_back(ConvertToNativeSeparators(file));
        }

        editTimes_.resize(files_.size());
        for (const std::string& file : files_) {
            editTimes_.emplace_back(std::filesystem::last_write_time(file));
        }
    }

    IReloadable::~IReloadable() = default;

    bool IReloadable::IsModified() const {
        for (int i = 0; i < files_.size(); ++i) {
            if (std::filesystem::last_write_time(files_[i]) != editTimes_[i]) {
                return true;
            }
        }

        return false;
    }

    void IReloadable::UpdateEditTimes() {
        for (int i = 0; i < files_.size(); ++i) {
            editTimes_[i] = std::filesystem::last_write_time(files_[i]);
        }
    }

    const std::vector<std::filesystem::file_time_type>& IReloadable::GetEditTimes() const {
        return editTimes_;
    }

}
