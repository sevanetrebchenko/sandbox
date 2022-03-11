
#include "common/api/shader/shader_preprocessor.h"

#include <utility>
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    ShaderInfo::ShaderInfo() : type(ShaderType::INVALID),
                               profile(ShaderProfile::INVALID),
                               version(-1, 0),
                               success(false)
                               {
    }

    ShaderPreprocessor::Tokenizer::Token::Token() : before(0),
                                                    after(0)
                                                    {
    }

    ShaderPreprocessor::Tokenizer::Token::Token(std::string in) : before(0),
                                                                  after(0)
                                                                  {
        length = in.length();

        // Determine number of whitespace characters before start of token.
        for (unsigned i = 0; i < length; ++i) {
            if (!std::isspace(in[i])) {
                before = i;
                break;
            }
        }

        // Determine number of whitespace characters after start of token.
        for (unsigned i = before; i < length; ++i) {
            if (std::isspace(in[i])) {
                after = length - i;
                break;
            }
        }

        // Remove any extra white space characters (\n, ' ', \r, \t, \f, \v)
        in.erase(std::remove_if(in.begin(), in.end(), [](char c) {
            return (c == '\n' || c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v');
        }), in.end());

        data = std::move(in);
        length = data.size();
    }

    unsigned ShaderPreprocessor::Tokenizer::Token::Size() const {
        return before + length + after;
    }

    ShaderPreprocessor::Tokenizer::Token::~Token() = default;

    ShaderPreprocessor::Tokenizer::Tokenizer() : size(0),
                                                 index(0)
                                                 {
    }

    ShaderPreprocessor::Tokenizer::~Tokenizer() = default;

    void ShaderPreprocessor::Tokenizer::Set(std::string in) {
        line = std::move(in);
        size = line.size();
        index = 0;
    }

    bool ShaderPreprocessor::Tokenizer::IsValid() const {
        if (index == size) {
            return false;
        }

        // EOF should also be triggered if the rest of the line is spaces.
        for (unsigned i = index; i < size; ++i) {
            if (!std::isspace(line[i])) {
                return true;
            }
        }

        return false;
    }

    ShaderPreprocessor::Tokenizer::Token ShaderPreprocessor::Tokenizer::Next() {
        if (!IsValid()) {
            // Reached end of tokenizer.
            return Token("");
        }

        unsigned current = index;
        bool foundCharacter = !std::isspace(line[current]);

        while (true) {
            if (current == size) {
                // Reached end of file parsing for next token.
                break;
            }

            foundCharacter |= !std::isspace(line[current]);
            ++current;

            // Substring [index, current] holds all non-whitespace characters.
            if (foundCharacter && std::isspace(line[current])) {
                // Encountered first whitespace character.
                break;
            }
        }

        // Continue until the next non-whitespace character.
        while (true) {
            if (current == size) {
                // Reached end of file parsing for next token.
                break;
            }

            if (!std::isspace(line[current])) {
                // Encountered first non-whitespace character.
                break;
            }

            ++current;
        }

        Token token(line.substr(index, current - index));
        index = current;
        return token;
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

            for (const std::string& warning : info.warnings) {
                std::cout << warning << std::endl;
            }

            for (const std::string& error : info.errors) {
                std::cout << error << std::endl;
            }

            std::cout << info.source << std::endl;

            if (!success) {
                exit(1);
            }
            else {
                exit(0);
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
        std::stringstream builder; // For building error messages.

        Tokenizer tokenizer;
        Tokenizer::Token token;
        std::string line;
        unsigned lineNumber = 1;
        unsigned offset = 0;

        while (!reader.eof()) {
            line = GetLine(reader);
            if (line.empty()) {
                goto unchanged;
            }

            tokenizer.Set(line);
            token = tokenizer.Next();
            offset = 0;

            if (token.data == "#") {
                // Spaces are allowed between the '#' and "include' / 'pragma' directives, as long as they are on the same line.
                if (!tokenizer.IsValid()) {
                    // '#' character on a single line is not allowed.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Invalid preprocessor directive.", offset + token.before));
                    info.success = false;

                    return false;
                }
                offset += token.Size(); // Offset starts at the first character of the next token.

                // Preprocessor directive.
                token = tokenizer.Next();

                if (token.data == "include") {
                    goto include;
                }
                else if (token.data == "version") {
                    goto version;
                }
                else if (token.data == "type") {
                    goto type;
                }
                else {
                    goto unchanged;
                }
            }
            else if (token.data == "#include") {
                goto include;
            }
            else if (token.data == "#version") {
                goto version;
            }
            else if (token.data == "#type") {
                goto type;
            }
            else {
                goto unchanged;
            }

            include: {
                if (!tokenizer.IsValid()) {
                    // #include must have filepath token to include.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME.", offset + token.length + 1));
                    info.success = false;

                    return false;
                }
                offset += token.Size();

                token = tokenizer.Next();
                assert(token.length > 0);

                // Process include token for correct formatting.
                bool first = token.data[0] == '\"';
                bool last = token.data[token.length - 1] == '\"';

                if ((first && !last) || (!first && last)) {
                    // Filepath needs to be either be surrounded by "" or have no decoration.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Include filepath must be in the format \"FILENAME\" or FILENAME.", offset, token.length));
                    info.success = false;

                    return false;
                }

                std::string filepath = token.data;
                if (first && last) {
                    filepath.erase(std::remove(filepath.begin(), filepath.end(), '\"'), filepath.end());
                }

                // TODO: this needs to be updated when caching / builds with asset directories are implemented.
                filepath = ConvertToNativeSeparators(filepath);

                // Check for extraneous characters after the #include filepath.
                if (tokenizer.IsValid()) {
                    unsigned numExtraCharacters = 0;
                    while (tokenizer.IsValid()) {
                        numExtraCharacters += tokenizer.Next().Size();
                    }

                    info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Extra tokens at the end of #version directive will be ignored.", offset + token.Size(), numExtraCharacters));
                }

                if (!Exists(filepath)) {

                    if (GetAssetDirectory(filepath).empty()) {
                        // Asset has no directory - include local to another directory.
                        std::string name = GetAssetName(filepath);
                        std::string extension = GetAssetExtension(filepath);

                        if (extension.empty()) {
                            offset += name.size();
                            if (filepath.find_last_of('.') != std::string::npos) {
                                // Extension has trailing dot.
                                offset += 1;
                            }

                            info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Invalid file extension for included file.", offset));
                            info.success = false;

                            return false;
                        }

                        // Construct list of paths to try to find the file.
                        std::set<std::string> allIncludeDirectories;

                        // Get path of file local to the current shader's working directory, given it exists.
                        if (info.workingDirectory.empty()) {
                            allIncludeDirectories.emplace(ConvertToNativeSeparators(info.workingDirectory));
                        }

                        // Get path of file relative to all globally configured shader include directories.
                        for (const std::string& includeDirectory : includeDirectories_) {
                            allIncludeDirectories.emplace(ConvertToNativeSeparators(includeDirectory));
                        }

                        // Generate list of valid file path(s).
                        std::set<std::string> validIncludeDirectories;

                        for (const std::string& directory : allIncludeDirectories) {
                            builder.str("");
                            builder << directory << '/' << name << '.' << extension;

                            std::string path = ConvertToNativeSeparators(builder.str());

                            if (Exists(path)) {
                                validIncludeDirectories.emplace(path);
                            }
                        }

                        if (validIncludeDirectories.empty()) {
                            // Could not find file to include.
                            if (allIncludeDirectories.empty()) {
                                builder.str("");
                                builder << "File '" << filepath << "' not found. ";
                            }
                            else {
                                builder.str("");
                                builder << "File '" << filepath << "' not found in any configured shader include directories (";

                                auto start = allIncludeDirectories.begin();
                                auto end = std::prev(allIncludeDirectories.end());
                                while (start != end) {
                                    builder << '\'' << *start << "', ";
                                    ++start;
                                }
                                builder << '\'' << *start << "'). ";
                            }

                            builder << "Did you forget to add a custom include directory with ShaderPreprocessor::AddIncludeDirectory?";

                            info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, builder.str(), offset, token.length));
                            info.success = false;

                            return false;
                        }

                        unsigned size = validIncludeDirectories.size();

                        if (size > 1) {
                            // Ambiguity between multiple valid filepath entries.
                            builder.str("");
                            builder << "Ambiguity between multiple valid file paths (";

                            auto start = validIncludeDirectories.begin();
                            auto end = std::prev(validIncludeDirectories.end());
                            while (start != end) {
                                builder << '\'' << *start << "', ";
                                ++start;
                            }
                            builder << '\'' << *start << "') for file '" << filepath << "'. Shader files require unique names (for includes local to configured shader include directories) or must have their path explicitly specified.";

                            info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, builder.str(), offset, token.length));
                            info.success = false;

                            return false;
                        }

                        // Filepath is guaranteed to be valid.
                        filepath = *validIncludeDirectories.begin();
                    }
                    else {
                        // File not found.
                        builder.str("");
                        builder << "File '" << filepath << "' not found. Did you forget to add a custom include directory with ShaderPreprocessor::AddIncludeDirectory?";

                        info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, builder.str(), offset, token.length));
                        info.success = false;

                        return false;
                    }
                }

                ShaderInclude include { filepath, lineNumber };

                // Check for circular dependencies.
                // Circular dependencies are stored in the ProcessingContext and are currently being processed (incomplete).
                for (const ShaderInclude& data : context.includeStack) {
                    if (data == include) {
                        builder.str("");
                        builder << "Detected circular include of file '" << filepath << "' (original include found in file '" << data.filepath << "' on line " << data.lineNumber << ").";

                        info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber,builder.str(), offset, token.length));
                        info.success = false;

                        return false;
                    }
                }

                // Check for duplicate includes.
                // Duplicate includes are stored in the ShaderInfo and are completed include directives.
                auto it = info.dependencies.find(include);
                if (it != info.dependencies.end()) {
                    // File has already been included.
                    ShaderInclude dependency = *it;

                    builder.str("");
                    builder << "Duplicate include of file '" << filepath << "' encountered (original include found in file '" << dependency.filepath << "' on line " << dependency.lineNumber << ").";

                    info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, builder.str(), offset, token.length));
                }
                else {
                    // Register new "in-flight" dependency.
                    context.includeStack.emplace_back(include);

                    ShaderInfo includeInfo { };
                    includeInfo.filepath = filepath;
                    includeInfo.workingDirectory = GetAssetDirectory(filepath);
                    includeInfo.type = ToShaderType(GetAssetExtension(filepath)); // Could be invalid.

                    bool success = ReadFile(includeInfo, context);

                    for (const std::string& warning : includeInfo.warnings) {
                        info.warnings.emplace_back(warning);
                    }

                    for (const std::string& error : includeInfo.errors) {
                        info.errors.emplace_back(error);
                    }

                    if (!success) {
                        // Error messages get printed from where the error occurred.
                        return false;
                    }

                    // Update 'info' struct with data from parsed file.
                    file << includeInfo.source;
                    info.dependencies.emplace(include);

                    // Remove "in-flight" dependency.
                    context.includeStack.pop_back();
                }

                ++lineNumber;
                continue;
            }

            version:
            {
                if (!tokenizer.IsValid()) {
                    // # version must have shader version number.
                    info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "#version directive missing version number.", offset + token.length + 1, 3));
                    info.success = false;

                    return false;
                }

                offset += token.Size();

                // Get shader version number.
                token = tokenizer.Next();
                ShaderVersion version { std::stoi(token.data), lineNumber };

                if (info.version.data != -1) {
                    // Shader already has versioning information.
                    if (version != info.version) {
                        // Encountered different shader version.
                        info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Shader version mismatch, using shader version (" + std::to_string(info.version.data) + ") found on line " + std::to_string(info.version.lineNumber) + ".", offset, token.length));
                    }
                }
                else {
                    // Shader will be the version of the first found #version directive.
                    if (!ValidateShaderVersion(version.data)) {
                        info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Invalid shader version (received '" + token.data + "', expecting 110, 120, 130, 140, 150, 330, 400, 410, 420, 430, 440, 450, or 460).", offset, token.length));
                        info.success = false;

                        return false;
                    }

                    info.version = version;

                    // Determine which version context to use (optional).
                    if (tokenizer.IsValid()) {
                        offset += token.Size();

                        // Explicitly provided shader profile.
                        token = tokenizer.Next();

                        if (!ValidateShaderProfile(token.data)) {
                            info.errors.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Invalid shader profile (received '" + token.data + "', expecting 'core' or 'compatibility').", offset, token.length));
                            info.success = false;

                            return false;
                        }

                        info.profile = ToShaderProfile(token.data);
                        offset += token.Size();

                        // Check for extraneous characters after the #version directive.
                        if (tokenizer.IsValid()) {
                            unsigned numExtraCharacters = 0;
                            while (tokenizer.IsValid()) {
                                numExtraCharacters += tokenizer.Next().Size();
                            }

                            info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Extra tokens at the end of #version directive will be ignored.", offset, numExtraCharacters));
                            file << line.substr(0, offset) << std::endl; // Remove extra characters at the end of line.
                        }
                        else {
                            file << line << std::endl;
                        }
                    }
                    else {
                        // Shader profile not provided, default to ShaderProfile::CORE.

                        // Manual offset: should point to the character directly after the shader version number, regardless of how many additional whitespace characters there are at the end of the line.
                        info.warnings.emplace_back(GetFormattedMessage(context, info.filepath, line, lineNumber, "Encountered eol when expecting shader profile - defaulting to 'core'.", offset + token.length + 1));
                        info.profile = ShaderProfile::CORE;

                        // Ensure proper spacing when appending shader profile to source file.
                        file << line;
                        if (token.after == 0) {
                            // No spaces after shader number.
                            file << ' ';
                        }
                        file << ToString(info.profile) << std::endl;
                    }
                }

                ++lineNumber;
                continue;
            }

            type:
            {

            }

            unchanged:
            {
                // Type of token is not processed, keep line unchanged.
                file << line << std::endl;
                ++lineNumber;
                continue;
            }
        }

        info.source = file.str();
        info.success = true;
        return true;
    }

    std::string ShaderPreprocessor::GetLine(std::ifstream& in) const {
        std::string out;
        std::getline(in, out);
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
        builder << ' ' << std::fixed << std::setw(4) << ' ' << " | " << std::setw(static_cast<int>(offset)) << ' ' << '^';

        if (length > 0) {
            for (unsigned i = 0; i < length - 1; ++i) {
                builder << '~';
            }
        }

        builder << std::endl;

        return builder.str();
    }

    void ShaderPreprocessor::AddIncludeDirectory(const std::string& in) {
        std::string includeDirectory = ConvertToNativeSeparators(in);

        if (includeDirectory.empty()) {
            return;
        }

        unsigned first = 0; // Index of first non-whitespace character.
        unsigned last = 0;  // Index of last non-whitespace character.

        // Find first non-whitespace character.
        unsigned size = includeDirectory.size();

        for (unsigned i = 0; i < size; ++i) {
            if (!std::isspace(includeDirectory[i])) {
                first = i;
                break;
            }
        }

        // First last non-whitespace character.
        for (unsigned i = size - 1; i >= 0; --i) {
            if (!std::isspace(includeDirectory[i])) {
                last = i;
                break;
            }
        }

        assert(last >= first);
        includeDirectory = includeDirectory.substr(first, (last - first) + 1);

        if (!Exists(includeDirectory)) {
            ImGuiLog::Instance().LogWarning("Directory '%s' provided to ShaderPreprocessor::AddIncludeDirectory does not exist.", includeDirectory.c_str());
            return;
        }

        // Directory is guaranteed to not be empty (empty directory does not exist).
        // Remove trailing '/' or '\\'.
        if (includeDirectory[includeDirectory.size() - 1] == '/' || includeDirectory[includeDirectory.size() - 1] == '\\') {
            includeDirectory = includeDirectory.substr(0, includeDirectory.size() - 1);
        }

        includeDirectories_.emplace(includeDirectory);
    }

}
