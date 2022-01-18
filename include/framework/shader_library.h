
#ifndef SANDBOX_SHADER_LIBRARY_H
#define SANDBOX_SHADER_LIBRARY_H

#include <sandbox_pch.h>
#include <framework/shader.h>
#include <framework/directory_utils.h>

namespace Sandbox {

    class ShaderLibrary {
        public:
            ShaderLibrary();
            ~ShaderLibrary();

            void AddShader(Shader* shader);
            void AddShader(const std::string& shaderName, const std::initializer_list<std::string>& shaderComponentPaths);

            void RecompileAllModified();
            void RecompileAll();

            Shader* GetShader(const std::string& shaderName);

        private:
            std::unordered_map<std::string, Shader*> _shaders;
            std::vector<IReloadable*> _recompileTargets;
    };

}

#endif //SANDBOX_SHADER_LIBRARY_H