
#include "common/api/shader/shader_component.h"
#include "common/utility/directory.h"
#include "common/utility/log.h"


namespace Sandbox {

    ShaderComponent::ShaderComponent(const std::string& filepath) :
                                                                    path_(ConvertToNativeSeparators(filepath)),
                                                                    baseDirectory_(GetAssetDirectory(filepath)),
                                                                    version_(-1)
                                                                    {
    }

    ShaderComponent::CompilationResult ShaderComponent::Compile() {
//        CompilationResult result;
//
//        source_ = Process(path_);
//        const GLchar* shaderSource = reinterpret_cast<const GLchar*>(source_.c_str());
//
//        // Create shader from source.
//        GLuint shader = glCreateShader(type_.ToOpenGLType());
//        glShaderSource(shader, 1, &shaderSource, nullptr); // If length is NULL, each string is assumed to be null terminated.
//        glCompileShader(shader);
//
//        // Compile shader source code.
//        GLint isCompiled = 0;
//        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
//        if (!isCompiled) {
//            // Shader failed to compile - get error information from OpenGL.
//            GLint errorMessageLength = 0;
//            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorMessageLength);
//
//            std::vector<GLchar> errorMessageBuffer;
//            errorMessageBuffer.resize(errorMessageLength + 1);
//            glGetShaderInfoLog(shader, errorMessageLength, nullptr, &errorMessageBuffer[0]);
//            std::string errorMessage(errorMessageBuffer.begin(), errorMessageBuffer.end());
//
//            glDeleteShader(shader);
//
//            result.success = false;
//            ++result.numErrors;
//            ImGuiLog::Instance().LogError("%s", errorMessage.c_str());
//        }
//        else {
//            result.success = true;
//            result.ID = shader;
//        }
//
//        return result;
    }

    int ShaderComponent::GetVersion() const {
        return version_;
    }

    const std::string& ShaderComponent::GetSource() const {
        return source_;
    }

    std::string ShaderComponent::Process(const std::string& filepath) {
        std::ifstream reader;
        reader.open(filepath);

        if (!reader.is_open()) {
            throw std::runtime_error("Could not open shader component: '" + filepath + "' for processing.");
        }

        // File holding the final output of shader processing.
        std::stringstream file;

        // Parser for processing individual lines.
        std::string line;
        unsigned lineNumber = 1;

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

            bool hasInclude = false;
            bool hasVersion = false;
            int length = 0;

            // Spaces are allowed between the '#' and "include' / 'pragma' directives, as long as they are on the same line.
            if (token == "#" || token == "# ") {
                length += static_cast<int>(token.length());

                if (tokenizer.eof()) {
                    // '#' character on a single line is not allowed.
                    ThrowFormattedError(line, lineNumber, "invalid preprocessor directive", length);
                }

                tokenizer >> token; // Preprocessor directive.

                if (token == "include") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(line, lineNumber, "#include directive missing \"FILENAME\"", length);
                    }

                    tokenizer >> token; // Filename.
                    hasInclude = true;
                }
                else if (token == "version") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(line, lineNumber, "#version directive missing version number", length);
                    }

                    // Version number.
                    tokenizer >> token;
                    hasVersion = true;
                }
            }
            else if (token == "#include") {
                length = static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME", length);
                }

                tokenizer >> token; // Filename.
                hasInclude = true;
            }
            else if (token == "#version") {
                length += static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(line, lineNumber, "#version directive missing version number", length);
                }

                // Version number.
                tokenizer >> token;
                hasVersion = true;
            }

            if (hasInclude) {
                token = ConvertToNativeSeparators(token);

                // Process given filepath for correct formatting.
                // Filepath can either be surrounded by "" or have no decoration.
                bool first = token[0] == '\"';
                bool last = token[token.length() - 1] == '\"';

                if ((first && !last) || (!first && last)) {
                    // Mismatch.
                    ThrowFormattedError(line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME", static_cast<int>(token.length()));
                }

                if (first && last) {
                    // Remove quotation marks.
                    token.erase(std::remove(token.begin(), token.end(), '\"'), token.end());
                }

                // Allow for includes to be local to the shader directory.
                std::string directory = GetAssetDirectory(token);
                std::string asset = GetAssetName(token);
                char native = GetNativeSeparator();

                // TODO: this needs to be updated when caching / builds with asset directories are a thing.
                if (directory.empty()) {
                    // Asset has no prepended directory (use local includes - directory of parent).
                    token = baseDirectory_ + native + asset;
                }

                ShaderInclude data { token, lineNumber };
                auto iterator = dependencies_.find(data);

                if (iterator != dependencies_.end()) {
                    unsigned original = iterator->lineNumber_;
//                    ++result_.numWarnings;
                    ImGuiLog::Instance().LogWarning("Duplicate include of file '%s' encountered on line %i (original include on line %i).", token.c_str(), lineNumber, original);

                    // Any repeats of already included files are ignored.
                    file << "// Comments auto-generated : file '" << token << "' included above." << std::endl;
                    file << "// " << line << std::endl;
                }
                else {
                    dependencies_.emplace(data);
                    file << Process(token) << std::endl;
                }
            }
            else if (hasVersion) {
                if (version_ != -1) {
                    int version = std::stoi(token);
                    if (version != version_) {
                        // Found different shader version number.
//                        ++result_.numWarnings;
                        ImGuiLog::Instance().LogWarning("Version mismatch (%i) encountered on line %i (shader is version %i).", version, lineNumber, version_);

                        // Include commented out version number.
                        file << "// Comments auto-generated : shader has version " << std::stoi(token) << "." << std::endl;
                        file << "// " << line << std::endl;
                    }
                    else {
                        file << "// " << line << std::endl;
                    }
                }
                else {
                    // Keep the first found shader version.
                    version_ = std::stoi(token);
                    file << line << std::endl;
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

    std::string ShaderComponent::GetLine(std::ifstream& stream) const {
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

    void ShaderComponent::ThrowFormattedError(const std::string& line, unsigned lineNumber, const std::string& message, int index) const {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << path_ + ':' + std::to_string(lineNumber) + ':' + std::to_string(index) + ':' + "error: " + message << std::endl;
        builder << std::fixed << std::setfill(' ') << std::setw(4) << std::to_string(lineNumber) << " | " << line << std::endl;
        builder << "     | " << std::setfill(' ') << std::setw(index) << "^" << std::endl;

        std::string error = builder.str();
        ImGuiLog::Instance().LogWarning(error.c_str());
        throw std::runtime_error(error);
    }

    ShaderComponent::ShaderInclude::ShaderInclude(const std::string& filename, unsigned int lineNumber) : filename_(filename),
                                                                                                          lineNumber_(lineNumber)
                                                                                                          {
    }

    bool ShaderComponent::ShaderInclude::operator==(const ShaderComponent::ShaderInclude& other) const {
    }

    ShaderComponent::ShaderInclude::~ShaderInclude() = default;

    std::size_t ShaderComponent::ShaderIncludeHash::operator()(const ShaderComponent::ShaderInclude& data) const noexcept {
    }

    ShaderComponent::CompilationResult::CompilationResult() : success(false),
                                                              numWarnings(0),
                                                              numErrors(0)
                                                              {
    }

    ShaderComponent::ProcessResult::ProcessResult() : numWarnings(0),
                                                      numErrors(0)
                                                      {
    }

}