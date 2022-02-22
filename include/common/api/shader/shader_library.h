
#pragma once

#include "pch.h"
#include "common/api/shader/shader.h"
#include "common/utility/reloadable.h"

namespace Sandbox {

    class ShaderLibrary {
        public:
            static ShaderLibrary& Instance();

            Shader* CreateShader(const std::string& name, const std::initializer_list<std::string>& shaderComponentPaths);
            void DestroyShader(const std::string& name);

            Shader* GetShader(const std::string& name) const;

            void RecompileModified();
            void RecompileAll();

        private:
            ShaderLibrary();
            ~ShaderLibrary();

            std::unordered_map<std::string, Shader*> shaders_;
            std::vector<IReloadable*> _recompileTargets;
    };

}
