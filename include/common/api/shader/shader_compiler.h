
#pragma once

#include "pch.h"
#include "common/utility/singleton.h"
#include "common/api/shader/shader_preprocessor.h"

namespace Sandbox {

    class ShaderCompiler : public ISingleton<ShaderCompiler> {
        public:
            REGISTER_SINGLETON(ShaderCompiler);

            // Note: function assumes 32-bit integer.
            [[nodiscard]] std::vector<unsigned> CompileToSPIRV(const ShaderInfo& shader);

        private:
            [[nodiscard]] shaderc_profile ToSPIRVShaderProfile(ShaderProfile profile) const;
            [[nodiscard]] shaderc_shader_kind ToSPIRVShaderType(ShaderType type) const;
    };

}