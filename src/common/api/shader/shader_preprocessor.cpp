
#include "common/api/shader/shader_preprocessor.h"
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {



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

    ShaderInfo::ShaderInfo(const std::string& filepath) : filepath(ConvertToNativeSeparators(filepath)),
                                                          workingDirectory(ConvertToNativeSeparators(GetAssetDirectory(filepath))),
                                                          type(ShaderType::INVALID),
                                                          context(ShaderContext::INVALID),
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
		ReadFile(filepath);
    }

    ShaderPreprocessor::ShaderPreprocessor() {
    }

    ShaderPreprocessor::~ShaderPreprocessor() {
    }

    void ShaderPreprocessor::ReadFile(const std::string& in) {
		std::string filepath = ConvertToNativeSeparators(in);

		if (parsed_.find(filepath) != parsed_.end()) {
			// File has already been parsed.
			return;
		}

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
            std::string context;

            // #type type
            bool hasType = false;
            std::string type;

            if (token == "#") {
                // Spaces are allowed between the '#' and "include' / 'pragma' directives, as long as they are on the same line.
                if (tokenizer.eof()) {
                    // '#' character on a single line is not allowed.
                    std::string error = GetFormattedMessage(filepath, lineNumber, "error: invalid preprocessor directive");
                }

                // Preprocessor directive.
                tokenizer >> token;
                token = RemoveWhitespace(token);

                if (token == "include") {
                    if (tokenizer.eof()) {
                        // # include must have filename to include.
                        std::string error = GetFormattedMessage(filepath, lineNumber, "error: #include directive missing \"FILENAME\"");
                    }

                    // Filename.
                    tokenizer >> include;
                    include = RemoveWhitespace(include);

                    hasInclude = true;
                }
                else if (token == "version") {
                    if (tokenizer.eof()) {
                        // # version must have shader version number.
                        std::string error = GetFormattedMessage(filepath, lineNumber, "error: #version directive missing version number");
                    }

                    // Version.
                    tokenizer >> token;
                    token = RemoveWhitespace(token);
                    version = std::stoi(token);

                    hasVersion = true;

                    // Determine which version context to use (optional).
                    if (!tokenizer.eof()) {
                        // Shader source explicitly provided version context.
                        tokenizer >> context;
                        context = RemoveWhitespace(context);
                    }
                }
                else if (token == "type") {
                    if (tokenizer.eof()) {
                        // # type must provide a (valid) shader type.
                        std::string error = GetFormattedMessage(filepath, lineNumber, "error: #type directive missing shader type");
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
                    std::string error = GetFormattedMessage(filepath, lineNumber, "error: #include directive missing \"FILENAME\"");
                }

                // Filename.
                tokenizer >> include;
                include = RemoveWhitespace(include);

                hasInclude = true;
            }
            else if (token == "#version") {
                if (tokenizer.eof()) {
                    // #version must have shader version number.
                    std::string error = GetFormattedMessage(filepath, lineNumber, "error: #version directive missing version number");
                }

                // Version.
                tokenizer >> token;
                token = RemoveWhitespace(token);
                version = std::stoi(token);

                hasVersion = true;

                // Determine which version context to use (optional).
                if (!tokenizer.eof()) {
                    // Shader source explicitly provided version context.
                    tokenizer >> context;
                    context = RemoveWhitespace(context);
                }
            }
            else if (token == "#type") {
                if (tokenizer.eof()) {
                    // #type must provide a (valid) shader type.
                    std::string error = GetFormattedMessage(filepath, lineNumber, "error: #type directive missing shader type");
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
                    std::string error = GetFormattedMessage(filepath, lineNumber, "error: #include directive missing \"FILENAME\" or FILENAME");
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
                	// File has already been included.
                    std::string message = "warning: duplicate include of file '" + filepath + "' encountered (original include found on line " + std::to_string(iterator->lineNumber) + ").";
                    info.warnings.emplace_back(GetFormattedMessage(filepath, lineNumber, message));

                    // Any repeats of already included files are ignored.
                    file << "// Comments auto-generated : file '" << include << "' included above." << std::endl;
                    file << "// " << line << std::endl;
                }
                else {
                	// Register new include.
                	info.dependencies.emplace(includeData);
                	ReadFile(include);

                	// Include file.
                	file << parsed_.at(include).source << std::endl;
                }
            }
            else if (hasVersion) {
            	if (info.version != -1) {
            		// Shader already has versioning information.

            		if (info.version != version) {
            			// Encountered different shader version.
            			std::string message = "warning: version mismatch encountered (shader is version " + std::to_string(info.version) + ").";
            			info.warnings.emplace_back(GetFormattedMessage(filepath, lineNumber, message));

            			// Include commented out version number.
            			file << "// Comments auto-generated : shader has version " << std::stoi(token) << "." << std::endl;
            		}

            		file << "// " << line << std::endl;
            	}
            	else {
            		// Shader will be the version of the first found #version directive.
            		if (!ValidateShaderVersion(version)) {
            			std::string message = "error: invalid GLSL shader version: " + std::to_string(version);
            			info.errors.emplace_back(GetFormattedMessage(filepath, lineNumber, message));
            		}

            		if (context.empty()) {
            			// Empty shader profile uses 'core' by default.
            			context = ToString(ShaderContext::CORE);

            			std::string message = "warning: encountered implicit shader context - shader will have context 'core'.";
            			info.warnings.emplace_back(GetFormattedMessage(filepath, lineNumber, message));
            		}

            		info.version = version;
            		info.context = ToShaderContext(context);

            		file << line << std::endl;
            	}
            }
            else if (hasType) {
				if (!ValidateShaderType(type)) {
					std::string message = "error: encountered invalid shader type '" + type + "'.";
					info.errors.emplace_back(GetFormattedMessage(filepath, lineNumber, message));
				}

				// TODO:
            }
            else {
                // Type of token is not processed, keep line unchanged.
                file << line << std::endl;
            }

            line.clear();
            ++lineNumber;
        }

        info.source = file.str();
        info.numLines = lineNumber - 1;
        info.success = info.errors.empty();

        parsed_.emplace(filepath, info);
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

    std::string ShaderPreprocessor::GetFormattedMessage(const std::string& file, unsigned lineNumber, const std::string& message) const {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << file << ':' << std::to_string(lineNumber) << ": " << message << std::endl;

        return builder.str();
    }

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

}

// C:/Users/sevan/Desktop/Projects/sandbox/include/common/api/shader/shader_component.h:28:33: error: 'ShaderType' does not name a type; did you mean 'ShaderDataType'?
// 28 |    [[nodiscard]] const ShaderType& GetType() const;