
#include "common/api/shader/shader_preprocessor.h"
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    [[nodiscard]] bool ValidateShaderExtension(const std::string& in) {
        static std::vector<std::string> validShaderExtensions { "vert", "frag", "geom", "tess", "comp", "glsl" };
        std::string extension = ToLower(in);

        for (const std::string& ext : validShaderExtensions) {
            if (extension == ext) {
                return true;
            }
        }

        return false;
    }

    std::string GetLine(std::ifstream& stream) {

    }

    [[nodiscard]] std::string GetFormattedErrorMessage(const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, int offset) {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << file + ':' + std::to_string(lineNumber) + ':' + std::to_string(offset) + ':' + "error: " + message << std::endl;
        builder << std::fixed << std::setfill(' ') << std::setw(4) << std::to_string(lineNumber) << " | " << line << std::endl;
        builder << "     | " << std::setfill(' ') << std::setw(offset) << "^" << std::endl;

        return builder.str();
    }

    // Reads shader file and
    void ReadShaderFile(const std::string& in, ShaderInfo& out) {
        std::string filepath = ConvertToNativeSeparators(in);

        std::string extension = ToLower(GetAssetExtension(filepath));
        if (extension.empty()) {
            ImGuiLog::Instance().LogError("PreprocessShaderFile called on a directory or a file that has no extension (path '%s').", filepath.c_str());
            out.success = false;
        }

        if (!ValidateShaderExtension(extension)) {
            ImGuiLog::Instance().LogError("PreprocessShaderFile called on a file with an invalid extension (received '%s', expecting '.vert' for vertex shaders, '.frag' for fragment shaders, '.geom' for geometry shaders, '.tess' for tesselation shaders, '.comp' for compute shaders, or '.glsl' for joint shader files).", extension.c_str());
            throw std::runtime_error("PreprocessShaderFile called with invalid extension - see out/log.txt for more details.");
        }

        std::ifstream reader;
        reader.open(filepath);

        if (!reader.is_open()) {
            ImGuiLog::Instance().LogError("PreprocessShaderFile called on a file that does not exist ('%s').", filepath.c_str());
            throw std::runtime_error("PreprocessShaderFile called with invalid arguments - see out/log.txt for more details.");
        }
    }

    PreprocessResult PreprocessShaderFile(const std::string& in) {
        std::string filepath = ConvertToNativeSeparators(in);

        std::string extension = ToLower(GetAssetExtension(filepath));
        if (extension.empty()) {
            ImGuiLog::Instance().LogError("PreprocessShaderFile called on a directory or a file that has no extension (path '%s').", filepath.c_str());
            throw std::runtime_error("PreprocessShaderFile called with invalid arguments - see out/log.txt for more details.");
        }

        if (!ValidateShaderExtension(extension)) {
            ImGuiLog::Instance().LogError("PreprocessShaderFile called on a file with an invalid extension (received '%s', expecting '.vert' for vertex shaders, '.frag' for fragment shaders, '.geom' for geometry shaders, '.tess' for tesselation shaders, '.comp' for compute shaders, or '.glsl' for joint shader files).", extension.c_str());
            throw std::runtime_error("PreprocessShaderFile called with invalid extension - see out/log.txt for more details.");
        }

        if (!Exists(filepath)) {
            ImGuiLog::Instance().LogError("PreprocessShaderFile called on a file that does not exist ('%s').", filepath.c_str());
            throw std::runtime_error("PreprocessShaderFile called with invalid arguments - see out/log.txt for more details.");
        }

        PreprocessResult result { };
        PreprocessData data { };

        ReadShaderFile(filepath, data, result);

        return result;

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
                    ThrowFormattedError(filepath, line, lineNumber, "invalid preprocessor directive", length);
                }

                tokenizer >> token; // Preprocessor directive.

                if (token == "include") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(filepath, line, lineNumber, "#include directive missing \"FILENAME\"", length);
                    }

                    tokenizer >> token; // Filename.
                    hasInclude = true;
                }
                else if (token == "version") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(filepath, line, lineNumber, "#version directive missing version number", length);
                    }

                    // Version number.
                    tokenizer >> token;
                    hasVersion = true;
                }
                else if (token == "type") {

                }
            }
            else if (token == "#include") {
                length = static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(filepath, line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME", length);
                }

                tokenizer >> token; // Filename.
                hasInclude = true;
            }
            else if (token == "#version") {
                length += static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(filepath, line, lineNumber, "#version directive missing version number", length);
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
                    ThrowFormattedError(filepath, line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME", static_cast<int>(token.length()));
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

        return result;
    }

    ShaderInfo::ShaderInfo(const std::string& filepath) : filepath(ConvertToNativeSeparators(filepath)),
                                                          workingDirectory(ConvertToNativeSeparators(GetAssetDirectory(filepath))),
                                                          type(ShaderType::INVALID),
                                                          profile(ShaderProfile::CORE), // Core by default.
                                                          version(-1),
                                                          source(),
                                                          dependencies(),
                                                          numLines(0),
                                                          success(false),
                                                          warnings(),
                                                          errors()
                                                          {
    }

    void ShaderPreprocessor::Process(const std::string& filepath) {

    }

    ShaderPreprocessor::ShaderPreprocessor() {
    }

    ShaderPreprocessor::~ShaderPreprocessor() {
    }

    void ShaderPreprocessor::ReadFile(const std::string& filepath) {
        // File holding the final output of shader processing.
        std::stringstream file;

        // Parser for processing individual lines.
        std::string line;
        unsigned lineNumber = 1;

        std::stringstream tokenizer;
        std::string token;

        std::ifstream reader;
        reader.open(filepath);

        if (!reader.is_open()) {
            // Failed to open shader file.
            return;
        }

        ShaderInfo info(filepath);

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
            token = RemoveWhitespace(token); // Get token without any spaces.

            // #include "filename"
            bool hasInclude = false;
            std::string include;

            // #version number (profile)
            bool hasVersion = false;
            int version;
            std::string profile;

            // #type type
            bool hasType = false;
            std::string type;

            if (token == "#") {
                // Spaces are allowed between the '#' and "include' / 'pragma' directives, as long as they are on the same line.
                if (tokenizer.eof()) {
                    // '#' character on a single line is not allowed.
                    std::string error = GetFormattedErrorMessage(filepath, lineNumber, "invalid preprocessor directive");
                }

                // Preprocessor directive.
                tokenizer >> token;
                token = RemoveWhitespace(token);

                if (token == "include") {
                    if (tokenizer.eof()) {
                        // # include must have filename to include.
                        std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#include directive missing \"FILENAME\"");
                    }

                    // Filename.
                    tokenizer >> include;
                    include = RemoveWhitespace(include);

                    hasInclude = true;
                }
                else if (token == "version") {
                    if (tokenizer.eof()) {
                        // # version must have shader version number.
                        std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#version directive missing version number");
                    }

                    // Version.
                    tokenizer >> token;
                    token = RemoveWhitespace(token);
                    version = std::stoi(token);

                    hasVersion = true;

                    // Determine which version profile to use (optional).
                    if (!tokenizer.eof()) {
                        // Shader source explicitly provided version profile.
                        tokenizer >> profile;
                        profile = RemoveWhitespace(profile);
                    }
                }
                else if (token == "type") {
                    if (tokenizer.eof()) {
                        // # type must provide a (valid) shader type.
                        std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#type directive missing shader type");
                    }

                    // Shader type.
                    tokenizer >> type;
                    type = RemoveWhitespace(type);

                    hasType = true;
                }
            }
            else if (token == "#include") {
                if (tokenizer.eof()) {
                    // #include must have filename to include.
                    std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#include directive missing \"FILENAME\"");
                }

                // Filename.
                tokenizer >> include;
                include = RemoveWhitespace(include);

                hasInclude = true;
            }
            else if (token == "#version") {
                if (tokenizer.eof()) {
                    // #version must have shader version number.
                    std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#version directive missing version number");
                }

                // Version.
                tokenizer >> token;
                token = RemoveWhitespace(token);
                version = std::stoi(token);

                hasVersion = true;

                // Determine which version profile to use (optional).
                if (!tokenizer.eof()) {
                    // Shader source explicitly provided version profile.
                    tokenizer >> profile;
                    profile = RemoveWhitespace(profile);
                }
            }
            else if (token == "#type") {
                if (tokenizer.eof()) {
                    // #type must provide a (valid) shader type.
                    std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#type directive missing shader type");
                }

                // Shader type.
                tokenizer >> type;
                type = RemoveWhitespace(type);

                hasType = true;
            }


            // Token processing.
            if (hasInclude) {
                // Process given filepath for correct formatting.
                // Filepath can either be surrounded by "" or have no decoration.
                bool first = include[0] == '\"';
                bool last = include[include.length() - 1] == '\"';

                if ((first && !last) || (!first && last)) {
                    // Formatting mismatch.
                    std::string error = GetFormattedErrorMessage(filepath, lineNumber, "#include directive missing \"FILENAME\" or FILENAME");
                }

                if (first && last) {
                    // Remove quotation marks.
                    include.erase(std::remove(include.begin(), include.end(), '\"'), include.end());
                }

                // Allow for includes to be local to the shader directory.
                // TODO: this needs to be updated when caching / builds with asset directories are a thing.
                if (GetAssetDirectory(include).empty()) {
                    // Asset has no prepended directory (use the directory of the parent shader as the working directory to include from).
                    include = info.workingDirectory + GetNativeSeparator() + GetAssetName(include) + '.' + GetAssetExtension(include);
                }
                // Otherwise, assume provided directory is correct (will be checked elsewhere).

                ShaderInclude includeData { include, lineNumber };
                auto iterator = info.dependencies.find(includeData);

                if (iterator != info.dependencies.end()) {
                    unsigned original = iterator->lineNumber;
                    ++info;
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
            else if (hasType) {

            }
            else {
                // Type of token is not processed, keep line unchanged.
                file << line << std::endl;
            }

            line.clear();
            ++lineNumber;
        }

        return result;
    }

    std::string ShaderPreprocessor::GetLine(std::ifstream& in) const {
        std::string out;
        std::getline(in, out);
        return std::move(out);
    }

    std::string ShaderPreprocessor::RemoveNewlines(const std::string& in) const {
        std::string out = in;
        out.erase(std::remove(out.begin(), out.end(), '\n'), out.end());
        return std::move(out);
    }

    std::string ShaderPreprocessor::RemoveWhitespace(const std::string& in) const {
        std::string out;

        // Remove any extra white space characters (\n, ' ', \r, \t, \f, \v)
        std::unique_copy(in.begin(), in.end(), std::back_insert_iterator<std::string>(out), [](char a,char b) {
            return isspace(a) && isspace(b);
        });

        return std::move(out);
    }

    std::string ShaderPreprocessor::GetFormattedErrorMessage(const std::string& file, unsigned lineNumber, const std::string& message) const {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << file + ':' + std::to_string(lineNumber) + ':' + "error: " + message << std::endl;

        return builder.str();
    }

}

// C:/Users/sevan/Desktop/Projects/sandbox/include/common/api/shader/shader_component.h:28:33: error: 'ShaderType' does not name a type; did you mean 'ShaderDataType'?
// 28 |    [[nodiscard]] const ShaderType& GetType() const;