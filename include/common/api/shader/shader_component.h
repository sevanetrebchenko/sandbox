
#pragma once

#include "pch.h"
#include "common/api/shader/shader_types.h"

namespace Sandbox {

    class ShaderComponent {
        public:
            explicit ShaderComponent(const std::string& filepath);
            ~ShaderComponent() = default;

            [[nodiscard]] const ShaderType& GetType() const;
            [[nodiscard]] const std::string& GetCode() const;

        private:
            [[nodiscard]] std::string Process(const std::string& filepath);

            [[nodiscard]] std::string GetLine(std::ifstream& stream) const;
            void ThrowFormattedError(const std::string& line, int lineNumber, const std::string& message, int index) const;

            ShaderType type_;
            std::string path_;
            std::string baseDirectory_; // Directory parent shader file is based in.
            int version_;
            std::string code_;
            std::unordered_set<std::string> dependencies_;
    };

}