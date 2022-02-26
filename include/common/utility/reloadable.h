
#pragma once

#include "pch.h"

namespace Sandbox {

    class IReloadable {
        public:
            explicit IReloadable(const std::string& file);
            IReloadable(const std::initializer_list<std::string>& files);
            ~IReloadable();

            [[nodiscard]] bool IsModified() const;
            virtual void Recompile() = 0;

        protected:
            [[nodiscard]] const std::vector<std::filesystem::file_time_type>& GetEditTimes() const;

            // Updates recompile time.
            void UpdateEditTimes();

        private:
            std::vector<std::string> files_;
            std::vector<std::filesystem::file_time_type> editTimes_;
    };

}