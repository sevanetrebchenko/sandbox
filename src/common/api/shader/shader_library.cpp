
#include <utility>

#include "common/api/shader/shader_library.h"

namespace Sandbox {

    struct ShaderComponent {
        ShaderComponent(std::string code, GLenum type);
        ~ShaderComponent() = default;

        std::string code_;
        GLenum type_;
    };

    class Parser {
        public:
            Parser();
            ~Parser();

            void Process(const std::initializer_list<std::string>& shaderComponentPaths);

        private:
            [[nodiscard]] std::string ProcessFile(const std::string& filepath) const;

            [[nodiscard]] std::string GetLine(std::ifstream& stream) const;

            std::unordered_map<std::string, ShaderComponent> shaderComponents_;

            bool processingExistingInclude_;

            // Keeps track of all the included files to allow for double includes without throwing redefinition errors.
            std::unordered_set<std::string> includedFiles_;
    };

    ShaderComponent::ShaderComponent(std::string code, GLenum type) : code_(std::move(code)),
                                                                      type_(type)
                                                                      {
    }

    Parser::Parser() {
    }

    Parser::~Parser() {
    }

    void Parser::Process(const std::initializer_list<std::string>& shaderComponentPaths) {
        for (const std::string& filepath : shaderComponentPaths) {
            std::string code = ProcessFile(filepath);
            GLenum type = ShaderType(filepath).ToOpenGLType();

            shaderComponents_.emplace(filepath, ShaderComponent(code, type));
        }
    }

    std::string Parser::ProcessFile(const std::string& filepath) const {
        std::ifstream reader;
        reader.open(filepath);

        if (!reader.is_open()) {
            throw std::runtime_error("Could not open shader file: '" + filepath + "' for processing.");
        }

        // File holding the final output of shader processing.
        std::stringstream file;
        int lineNumber = 1;

        // Parser for processing individual lines.
        std::string line;

        std::stringstream tokenizer;
        std::string token;

//        while (!reader.eof()) {
//            line = GetLine(reader);
//
//            // Tokenize input line.
//            tokenizer.clear();
//            tokenizer.str(line);
//
//            while (!tokenizer.eof()) {
//                tokenizer >> token;
//                std::cout << token << std::endl;
//            }
//
//            std::cout << std::endl;
//            line.clear();
//        }

        return std::string();
    }

    std::string Parser::GetLine(std::ifstream& stream) const {
        std::string line;
        std::getline(stream, line);

        // Erase new lines.
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        // Erase comments.

        return std::move(line);
    }

    ShaderLibrary::ShaderLibrary() {
    }

    ShaderLibrary::~ShaderLibrary() {
        for (const auto& shaderPair : _shaders) {
            delete shaderPair.second;
        }
    }

    void ShaderLibrary::AddShader(Shader *shader) {
        _shaders.emplace(shader->GetName(), shader);
        _recompileTargets.push_back(shader);
    }

    void ShaderLibrary::AddShader(const std::string &shaderName, const std::initializer_list<std::string> &shaderComponentPaths) {
        Parser parser;
        parser.Process(shaderComponentPaths);

        Shader* shader = new Shader(shaderName, shaderComponentPaths);

        _shaders.emplace(shaderName, shader);
        _recompileTargets.push_back(shader);
    }

    Shader *ShaderLibrary::GetShader(const std::string &shaderName) {
        auto shaderIter = _shaders.find(shaderName);

        // Found shader in library.
        if (shaderIter != _shaders.end()) {
            return shaderIter->second;
        }

        return nullptr;
    }

    void ShaderLibrary::RecompileAllModified() {
        for (IReloadable* target : _recompileTargets) {
            target->RecompileIfModified();
        }
    }

    void ShaderLibrary::RecompileAll() {
        for (const auto& shaderPair : _shaders) {
            shaderPair.second->Recompile();
        }
    }

}