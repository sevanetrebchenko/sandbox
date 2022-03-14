
#pragma once

#include "pch.h"
#include "common/api/shader/shader.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class ShaderLibrary : public Singleton<ShaderLibrary> {
        public:
            REGISTER_SINGLETON(ShaderLibrary);

            Shader* CreateShader(const std::string& name, const std::string& filepath);
            Shader* CreateShader(const std::string& name, const std::initializer_list<std::string>& filepaths);

            void DestroyShader(const std::string& name);
            Shader* GetShader(const std::string& name) const;

            void RecompileModified();
            void RecompileAll();

        private:
            ShaderLibrary();
            ~ShaderLibrary() override;

            std::unordered_map<std::string, Shader*> shaders_;
    };

}
