
#pragma once

#include "pch.h"
#include "common/utility/singleton.h"
#include "common/api/shader/shader_preprocessor.h"

namespace Sandbox {

    class ShaderCompiler : public ISingleton<ShaderCompiler> {
        public:
            REGISTER_SINGLETON(ShaderCompiler);

            [[nodiscard]] std::vector<std::uint32_t> CompileToSPIRV(ShaderInfo& info);

        private:
            [[nodiscard]] shaderc_profile ToSPIRVShaderProfile(ShaderProfile profile) const;
            [[nodiscard]] shaderc_shader_kind ToSPIRVShaderType(ShaderType type) const;
    };

}