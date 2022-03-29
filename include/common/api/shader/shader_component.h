
#pragma once

#include "pch.h"
#include "common/api/shader/shader_types.h"
#include "common/utility/directory.h"

namespace Sandbox {

    class ShaderComponent {
        public:
            explicit ShaderComponent(const std::string& filepath);
            ~ShaderComponent() = default;

            [[nodiscard]] GLuint Compile() const;

        private:
            std::string filepath_;
    };

}





