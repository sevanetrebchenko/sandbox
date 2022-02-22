
#include "common/api/shader/shader_component.h"
#include "common/utility/directory_utils.h"
#include "common/utility/imgui_log.h"

namespace Sandbox {

    ShaderComponent::ShaderComponent(const std::string& filepath) : type_(GetAssetExtension(filepath)),
                                                                    path_(ConvertToNativeSeparators(filepath)),
                                                                    baseDirectory_(GetAssetDirectory(filepath)),
                                                                    version_(-1)
    {
        code_ = Process(path_);
        ImGuiLog::Instance().LogTrace("Loaded shader component '%s' (type: %s, version: %i)", path_.c_str(), type_.ToString().c_str(), version_);
        std::cout << code_ << std::endl;
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
        int lineNumber = 1;

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
                    ThrowFormattedError(line, lineNumber, "invalid preprocessor directive", length);
                }

                tokenizer >> token; // Preprocessor directive.

                if (token == "include") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(line, lineNumber, "#include directive missing \"FILENAME\"", length);
                    }

                    tokenizer >> token; // Filename.
                    include = true;
                }
                else if (token == "version") {
                    length += static_cast<int>(token.length());
                    if (tokenizer.eof()) {
                        ThrowFormattedError(line, lineNumber, "#version directive missing version number", length);
                    }

                    // Version number.
                    tokenizer >> token;
                    version = true;
                }
            }
            else if (token == "#include") {
                length = static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME", length);
                }

                tokenizer >> token; // Filename.
                include = true;
            }
            else if (token == "#version") {
                length += static_cast<int>(token.length());
                if (tokenizer.eof()) {
                    ThrowFormattedError(line, lineNumber, "#version directive missing version number", length);
                }

                // Version number.
                tokenizer >> token;
                version = true;
            }

            if (include) {
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

                auto iterator = dependencies_.find(token);
                if (iterator != dependencies_.end()) {
                    // Any repeats of already included files are ignored.
                    file << "// Comments auto-generated : file '" << token << "' included above." << std::endl;
                    file << "// " << line << std::endl;
                }
                else {
                    dependencies_.emplace(token);
                    file << Process(token) << std::endl;
                }
            }
            else if (version) {
                if (version_ == -1) {
                    // Keep the first found shader version.
                    version_ = std::stoi(token);
                    file << line << std::endl;
                }
                else {
                    // Include commented out version number.
                    file << "// Comments auto-generated : shader has version " << std::stoi(token) << "." << std::endl;
                    file << "// " << line << std::endl;
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

    void ShaderComponent::ThrowFormattedError(const std::string& line, int lineNumber, const std::string& message, int index) const {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << path_ + ':' + std::to_string(lineNumber) + ':' + std::to_string(index) + ':' + "error: " + message << std::endl;
        builder << std::fixed << std::setfill(' ') << std::setw(4) << std::to_string(lineNumber) << " | " << line << std::endl;
        builder << "     | " << std::setfill(' ') << std::setw(index) << "^" << std::endl;

        std::string error = builder.str();
        ImGuiLog::Instance().LogWarning(error.c_str());
        throw std::runtime_error(error);
    }

    const ShaderType& ShaderComponent::GetType() const {
        return type_;
    }

    const std::string& ShaderComponent::GetCode() const {
        return code_;
    }

}