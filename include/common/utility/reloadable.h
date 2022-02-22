
#pragma once

#include "pch.h"

namespace Sandbox {

    class IReloadable {
        public:
            IReloadable(const std::initializer_list<std::string>& files);
            explicit IReloadable(const std::string& file);
            ~IReloadable();

            [[nodiscard]] bool IsModified() const;
            virtual void Recompile() = 0;

        protected:
            // Updates recompile time.
            void Clear();

        private:
            std::vector<std::string> files_;
            std::vector<std::filesystem::file_time_type> modifyTimes_;
    };

}