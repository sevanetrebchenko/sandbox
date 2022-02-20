
#include <utility>

#include "common/api/shader/shader_library.h"
#include "common/utility/imgui_log.h"

namespace Sandbox {

    struct ShaderComponent {
        ShaderComponent(std::string code, GLenum type);
        ~ShaderComponent() = default;

        std::string path_;
        int version_;
        std::string code_;
        std::unordered_set<std::string> dependencies_;

        GLenum type_;
    };

    class Parser {
        public:
            Parser();
            ~Parser();

            void Process(const std::initializer_list<std::string>& shaderComponentPaths);

        private:
            [[nodiscard]] std::string ProcessFile(const std::string& filepath);

            [[nodiscard]] std::string GetLine(std::ifstream& stream) const;
            void ThrowFormattedError(const std::string& file, int lineNumber, const std::string& line, const std::string& message, int index) const;

            std::unordered_map<std::string, ShaderComponent> shaderComponents_;

            // Keeps track of all the included files to allow for double includes without throwing redefinition errors.
            std::unordered_set<std::string> includedFiles_;
            int version_;
    };

    ShaderComponent::ShaderComponent(std::string code, GLenum type) : code_(std::move(code)),
                                                                      type_(type)
                                                                      {
    }

    Parser::Parser() : version_(-1) {
    }

    Parser::~Parser() {
    }

    void Parser::Process(const std::initializer_list<std::string>& shaderComponentPaths) {
        for (const std::string& filepath : shaderComponentPaths) {
            std::string code = ProcessFile(filepath);

            std::cout << code << std::endl;

            GLenum type = ShaderType(filepath).ToOpenGLType();

            shaderComponents_.emplace(filepath, ShaderComponent(code, type));
        }
    }

    std::string Parser::ProcessFile(const std::string& filepath) {
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

        while (!reader.eof()) {
            line = GetLine(reader);

            // Tokenize input line.
            tokenizer.clear();
            tokenizer.str(line);

            if (line.empty()) {
                file << std::endl;
                continue;
            }

            tokenizer >> token;

            bool include = false;
            bool version = false;
            int length = 0;

            // Spaces are allowed between the '#' and "include' / 'pragma' directives, as long as they are on the same line.
            if (token == "#" || token == "# ") {
                length += static_cast<int>(token.length());

                if (tokenizer.eof()) {
                    // '#' character on a single line is not allowed.
                    ThrowFormattedError(filepath, lineNumber, line, "invalid preprocessor directive", length);
                }

                tokenizer >> token; // Preprocessor directive.

                if (token == "include") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(filepath, lineNumber, line, "#include directive missing \"FILENAME\"", length);
                    }

                    tokenizer >> token; // Filename.
                    include = true;
                }
                else if (token == "version") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(filepath, lineNumber, line, "#version directive missing version number", length);
                    }

                    // Version number.
                    tokenizer >> token;
                    version = true;
                }
            }
            else if (token == "#include") {
                length = static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(filepath, lineNumber, line, "#include directive missing \"FILENAME\" or FILENAME", length);
                }

                tokenizer >> token; // Filename.
                include = true;
            }
            else if (token == "#version") {
                length += static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(filepath, lineNumber, line, "#version directive missing version number", length);
                }

                // Version number.
                tokenizer >> token;
                version = true;
            }

            if (include) {
                auto iterator = includedFiles_.find(token);
                if (iterator != includedFiles_.end()) {
                    // Any repeats of already included files are ignored.
                    // TODO: issue warning.
                    file << std::endl;
                }
                else {
                    includedFiles_.emplace(token);

                    // Process given filepath for correct formatting.
                    // Filepath can either be surrounded by "" or have no decoration.
                    bool first = token[0] == '\"';
                    bool last = token[token.length() - 1] == '\"';

                    if ((first && !last) || (!first && last)) {
                        // Mismatch.
                        ThrowFormattedError(filepath, lineNumber, line, "#include directive missing \"FILENAME\" or FILENAME", static_cast<int>(token.length()));
                    }

                    if (first && last) {
                        // Remove quotation marks.
                        token.erase(std::remove(token.begin(), token.end(), '\"'), token.end());
                    }

                    // Allow for includes to be local to the shader directory.

                    file << ProcessFile(token) << std::endl;
                }
            }
            else if (version) {
                if (version_ == -1) {
                    // Keep the first found shader version.
                    version_ = std::stoi(token);
                }
            }
            else {
                // Type of token is not processed, keep line unchanged.
                file << line << std::endl;
            }

            line.clear();
            ++lineNumber;
        }

        return file.str();
    }

    std::string Parser::GetLine(std::ifstream& stream) const {
        std::string line;
        std::getline(stream, line);

        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

//        std::string output;
//
//        // Remove any extra white space characters (\n, ' ', \r, \t, \f, \v)
//        std::unique_copy(line.begin(), line.end(), std::back_insert_iterator<std::string>(output), [](char a,char b) {
//            return isspace(a) && isspace(b);
//        });

        return std::move(line);
    }

    void Parser::ThrowFormattedError(const std::string& file, int lineNumber, const std::string& line, const std::string& message, int index) const {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << file + ':' + std::to_string(lineNumber) + ':' + std::to_string(index) + ':' + "error: " + message << std::endl;
        builder << std::fixed << std::setfill(' ') << std::setw(4) << std::to_string(lineNumber) << " | " << line << std::endl;
        builder << "     | " << std::setfill(' ') << std::setw(index) << "^" << std::endl;

        std::string error = builder.str();
        ImGuiLog::Instance().LogWarning(error.c_str());
        throw std::runtime_error(error);
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