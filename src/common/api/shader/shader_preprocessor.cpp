
#include "common/api/shader/shader_preprocessor.h"
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    ShaderInfo::ShaderInfo() : type(ShaderType::INVALID),
                               profile(ShaderProfile::INVALID),
                               version(-1),
                               success(false)
                               {
    }

    std::unordered_map<ShaderType, ShaderInfo> ShaderPreprocessor::ProcessFile(const std::string& in) {
        std::string filepath = ConvertToNativeSeparators(in);
        std::string extension = ToLower(GetAssetExtension(filepath));

        if (!ValidateShaderExtension(extension)) {
            ImGuiLog::Instance().LogError("ShaderPreprocessor::ReadFile called on a file with an invalid extension (received '%s', expecting '.vert' for vertex shaders, '.frag' for fragment shaders, '.geom' for geometry shaders, '.tess' for tesselation shaders, '.comp' for compute shaders, or '.glsl' for joint shader files).", extension.c_str());
            throw std::runtime_error("ShaderPreprocessor::ReadFile called on a file with an invalid extension - see out/log.txt for more details.");
        }

        if (!Exists(filepath)) {
            ImGuiLog::Instance().LogError("ShaderPreprocessor::ReadFile called on a file that does not exist ('%s').", filepath.c_str());
            throw std::runtime_error("ShaderPreprocessor::ReadFile called on a file with that does not exist - see out/log.txt for more details.");
        }

        if (extension == "glsl") {
            // Processing joint shader file.
        }
        else {
            // Processing individual shader file.
            ShaderInfo info { };
            info.filepath = filepath;
            info.workingDirectory = GetAssetDirectory(filepath);

            info.type = ToShaderType(extension);
            assert(info.type != ShaderType::INVALID); // Verified above.

            ProcessingContext context { };
            bool success = ReadFile(info, context);

            if (!success) {
                info.success = false;
            }
        }
    }

    ShaderPreprocessor::ShaderPreprocessor() {
    }

    ShaderPreprocessor::~ShaderPreprocessor() {
    }

    bool ShaderPreprocessor::ReadFile(ShaderInfo& info, ProcessingContext& context) {
        std::ifstream reader;
        reader.open(info.filepath);
        assert(reader.is_open());

        // File is guaranteed to exist and be a valid file to include (no circular/duplicate include).

        // Stream to hold the contents of the output file.
        std::stringstream file;

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

            unsigned offset = 0; // Offset at which to place the '^' character in the error message.

            tokenizer >> token;
            offset += token.length();
            token = RemoveWhitespace(token); // Get token without any spaces.

            if (token == "#") {
                // Spaces are allowed between the '#' and "include' / 'pragma' directives, as long as they are on the same line.
                if (tokenizer.eof()) {
                    // '#' character on a single line is not allowed.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"error: invalid preprocessor directive", offset));
                    info.success = false;

                    return false;
                }

                // Preprocessor directive.
                tokenizer >> token;
                offset += token.length();
                token = RemoveWhitespace(token);

                if (token == "include") {
                    goto include;
                }
                else if (token == "version") {
                    goto version;
                }
                else if (token == "type") {
                    goto type;
                }
                else {
                    goto out;
                }
            }
            else if (token == "#include") {
                goto include;
            }
            else if (token == "#version") {
                goto version;
            }
            else if (token == "#type") {
                goto type;
            }
            else {
                goto out;
            }

            include: {
                if (tokenizer.eof()) {
                    // #include must have filepath token to include.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"error: #include directive missing \"FILENAME\"", offset));
                    info.success = false;

                    return false;
                }

                tokenizer >> token;
                offset += token.length();
                token = RemoveWhitespace(ConvertToNativeSeparators(token));
                unsigned length = token.length();

                // Process include token for correct formatting.
                bool first = token[0] == '\"';
                bool last = token[length - 1] == '\"';

                if ((first && !last) || (!first && last)) {
                    // Filepath needs to be either be surrounded by "" or have no decoration.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"error: filepath to include must must be in the format \"FILENAME\" or FILENAME", offset, length));
                    info.success = false;

                    return false;
                }

                if (first && last) {
                    token.erase(std::remove(token.begin(), token.end(), '\"'), token.end());
                    length = token.length();
                }

                // Check for extraneous characters after the #include filepath.
                if (!tokenizer.eof()) {
                    unsigned numExtraCharacters = 0;
                    while (!tokenizer.eof()) {
                        tokenizer >> token;
                        numExtraCharacters += token.length();
                    }

                    info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"warning: extra tokens at the end of #include directive", offset, numExtraCharacters));
                }

                // Ensure global include path is valid and exists.
                // TODO: this needs to be updated when caching / builds with asset directories are implemented.
                if (GetAssetDirectory(token).empty()) {
                    // Allow for includes to be local to the shader directory.
                    token = ConvertToNativeSeparators(info.workingDirectory + GetNativeSeparator() + GetAssetName(token) + '.' + GetAssetExtension(token));
                }

                if (!Exists(token)) {
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"error: file '" + token + "' not found", offset, length));
                    info.success = false;

                    return false;
                }

                ShaderInclude includeData { token, lineNumber };

                // Check for circular dependencies.
                // Circular dependencies are stored in the ProcessingContext and are currently being processed (incomplete).
                for (const ShaderInclude& data : context.includeStack) {
                    if (data == includeData) {
                        info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"error: detected circular include of file '" + token + "' (original include found in file '" + data.filepath + "' on line " + std::to_string(data.lineNumber) + ")", offset, length));
                        info.success = false;

                        return false;
                    }
                }

                // Check for duplicate includes.
                // Duplicate includes are stored in the ShaderInfo and are completed include directives.
                auto dependency = info.dependencies.find(includeData);
                if (dependency != info.dependencies.end()) {
                    // File has already been included.
                    info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "warning: duplicate include of file '" + token + "' encountered (original include found in file '" + dependency->filepath + "' on line " + std::to_string(dependency->lineNumber) + ")", offset, length));

                    // Any repeats of already included files are ignored.
                    file << "// Comment auto-generated : file '" << token << "' included above." << std::endl;
                    file << "// " << line << std::endl;
                }
                else {
                    // Register new "in-flight" dependency.
                    context.includeStack.emplace_back(includeData);

                    if (!ReadFile(info, context)) {
                        return false;
                    }

                    // Remove "in-flight" dependency and register as complete include.
                    for (auto it = context.includeStack.begin(); it != context.includeStack.end(); ++it) {
                        const ShaderInclude& data = *it;
                        if (data == includeData) {
                            context.includeStack.erase(it);
                            break;
                        }
                    }
                    info.dependencies.emplace(includeData);
                }
            }

            version:
            {
                if (tokenizer.eof()) {
                    // # version must have shader version number.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"error: #version directive missing version number", offset));
                    info.success = false;

                    return false;
                }

                // Get shader version number.
                tokenizer >> token;
                offset += token.length();
                token = RemoveWhitespace(ConvertToNativeSeparators(token));
                unsigned length = token.length();

                int version = std::stoi(token);

                if (info.version != -1) {
                    // Shader already has versioning information.

                    if (info.version != version) {
                        // Encountered different shader version.
                        info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "warning: version mismatch encountered (shader is version " + std::to_string(info.version) + ")", offset, length));

                        // Include commented out version number.
                        file << "// Comment auto-generated : shader has version " << std::to_string(info.version) << "." << std::endl;
                    }
                }
                else {
                    // Shader will be the version of the first found #version directive.
                    if (!ValidateShaderVersion(version)) {
                        info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "error: invalid GLSL shader version (received '" + token + "', expecting 110, 120, 130, 140, 150, 330, 400, 410, 420, 430, 440, 450, or 460)", offset, length));
                        info.success = false;

                        return false;
                    }

                    info.version = version;

                    // Determine which version context to use (optional).
                    if (!tokenizer.eof()) {
                        // Explicitly provided shader profile.
                        tokenizer >> token;
                        offset += token.length();
                        token = RemoveWhitespace(token);
                        length = token.length();

                        if (!ValidateShaderProfile(token)) {
                            info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "error: invalid GLSL shader profile (received '" + token + "', expecting 'core' or 'compatibility')", offset, length));
                            info.success = false;

                            return false;
                        }

                        info.profile = ToShaderProfile(token);

                        // Check for extraneous characters after the #version directive.
                        if (!tokenizer.eof()) {
                            unsigned numExtraCharacters = 0;
                            while (!tokenizer.eof()) {
                                tokenizer >> token;
                                numExtraCharacters += token.length();
                            }

                            info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,"warning: extra tokens at the end of #version directive", offset, numExtraCharacters));
                        }
                    }
                    else {
                        // Shader profile not provided, default to ShaderProfile::CORE.
                        info.profile = ShaderProfile::CORE;
                        info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "warning: encountered implicit shader context - shader will have context 'core'", offset));
                    }

                    file << "#version " << std::to_string(info.version) << " " << ToString(info.profile) << std::endl;
                }
            }

            type:
            {

            }

            out:
            {
                // Type of token is not processed, keep line unchanged.
                file << line << std::endl;
            }

            line.clear();
            ++lineNumber;
        }

        info.source = file.str();
        return true;
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

    // Used for validating shader file extensions.
    [[nodiscard]] bool ShaderPreprocessor::ValidateShaderExtension(const std::string& in) const {
    	static std::vector<std::string> validShaderExtensions { "vert", "frag", "geom", "tess", "comp", "glsl" };
    	std::string extension = ToLower(in);

    	for (const std::string& valid : validShaderExtensions) {
    		if (extension == valid) {
    			return true;
    		}
    	}

    	return false;
    }

    // Used for validating #version ... preprocessor directives.
    [[nodiscard]] bool ShaderPreprocessor::ValidateShaderVersion(int version) const {
    	// Versioning information taken from https://en.wikipedia.org/wiki/OpenGL_Shading_Language.
    	static std::vector<int> validShaderVersions { 110, 120, 130, 140, 150, 330, 400, 410, 420, 430, 440, 450, 460 };

    	for (int valid : validShaderVersions) {
    		if (version == valid) {
    			return true;
    		}
    	}

    	return false;
    }

    bool ShaderPreprocessor::ValidateShaderProfile(const std::string& in) const {
        static std::vector<std::string> validShaderProfiles { "core", "compatibility" };
        std::string profile = ToLower(in);

        for (const std::string& valid : validShaderProfiles) {
            if (profile == valid) {
                return true;
            }
        }

        return false;
    }

    // Used for validating #type ... preprocessor directives (joint .glsl shader files).
	bool ShaderPreprocessor::ValidateShaderType(const std::string& in) const {
		static std::vector<std::string> validShaderTypes { "vert", "vertex",
														   "frag", "fragment",
														   "geom", "geometry",
														   "comp", "compute" };
		std::string type = ToLower(in);

		for (const std::string& valid : validShaderTypes) {
			if (type == valid) {
				return true;
			}
		}

		return false;
	}

    std::string ShaderPreprocessor::GetFormattedMessage(const ProcessingContext& context, const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, unsigned offset, unsigned length) const {
        std::stringstream builder;

        // Formatted how typical compiler errors are formatted:

        // In file included from [file1]:[include line number],
        //                  from [file2]:[include line number],
        //                  from [file3]:[include line number]:
        // [file]:[line number]:[column offset]: [error message]
        //  #### | [line on which error occurred]
        //       |                ^~~~~~~~

        std::size_t size = context.includeStack.size();

        // Build up include stack in message.
        if (!context.includeStack.empty()) {
            builder << "In file included from " << context.includeStack[0].filepath << ':' << context.includeStack[0].lineNumber;

            if (size > 1) {
                builder << ',' << std::endl;

                // Align any additional messages to the first line.
                for (int i = 1; i < size - 1; ++i) {
                    builder << "                 from " << context.includeStack[i].filepath << ':' << context.includeStack[i].lineNumber << ',' << std::endl;
                }
                builder << "                 from " << context.includeStack[size - 1].filepath << ':' << context.includeStack[size - 1].lineNumber;
            }

            builder << ':' << std::endl;
        }

        builder << file << ':' << lineNumber << ':' << offset << ": " << message << std::endl;
        builder << ' ' << std::fixed << std::setw(4) << lineNumber << " | " << line << std::endl;
        builder << ' ' << std::fixed << std::setw(4) << ' ' << " | " << std::setw(static_cast<int>(offset)) << ' ' << '^' << std::setw(static_cast<int>(length)) << '~' << std::endl;

        return builder.str();
    }

}
