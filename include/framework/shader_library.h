
#ifndef SANDBOX_SHADER_LIBRARY_H
#define SANDBOX_SHADER_LIBRARY_H

#include <sandbox.h>
#include <framework/shader.h>
#include <framework/directory_utils.h>
#include <framework/singleton.h>

namespace Sandbox {

    class ShaderLibrary : public Singleton<ShaderLibrary> {
        public:
            REGISTER_SINGLETON(ShaderLibrary);

            void Initialize() override;
            void Shutdown() override;

            void AddShader(Shader* shader);
            void AddShader(const std::string& shaderName, const std::initializer_list<std::string>& shaderComponentPaths);

            void RecompileAllModified();
            void RecompileAll();

            Shader* GetShader(const std::string& shaderName);

            void Clear();

        private:
            ShaderLibrary();
            ~ShaderLibrary() override;

            std::unordered_map<std::string, Shader*> _shaders;
            std::vector<IReloadable*> _recompileTargets;
    };

}

#endif //SANDBOX_SHADER_LIBRARY_H