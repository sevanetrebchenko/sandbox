
#include "common/api/shader/shader_preprocessor.h"

#include <utility>
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    class Token {
        public:
            Token();
            explicit Token(std::string in);
            Token& operator=(std::string in);
            ~Token();

            [[nodiscard]] unsigned size() const;

            std::string data; // Token with no whitespace.
            unsigned before;  // Number of whitespace characters before token (if any).

        private:
            void Process(std::string in);
    };

    Token::Token() : before(0) {
    }

    Token::Token(std::string in) : Token() {
        Process(std::move(in));
    }

    Token::~Token() = default;

    Token& Token::operator=(std::string in) {
        Process(std::move(in));
        return *this;
    }

    void Token::Process(std::string in) {
        // Determine number of whitespace characters before start of token.
        for (unsigned i = 0; i < in.length(); ++i) {
            if (!std::isspace(in[i])) {
                before = i;
                break;
            }
        }

        // Remove any extra white space characters (\n, ' ', \r, \t, \f, \v)
        in.erase(std::remove_if(in.begin(), in.end(), [](char c) {
            return (c == '\n' || c == ' ' || c == '\r' || c == '\t' || c == '\f' || c == '\v');
        }), in.end());

        data = std::move(in);
    }

    unsigned Token::size() const {
        return before + data.size();
    }



    // Custom string tokenizer that does not ignore whitespace characters.
    class Tokenizer {
        public:
            Tokenizer();
            ~Tokenizer();

            void str(std::string in);
            [[nodiscard]] bool eof() const;
            void operator>>(Token& token);

        private:
            std::string line;
            unsigned index;
    };

    Tokenizer::Tokenizer() : index(0) {
    }

    Tokenizer::~Tokenizer() = default;

    void Tokenizer::str(std::string in) {
        line = std::move(in);
        index = 0;
    }

    bool Tokenizer::eof() const {
        unsigned size = line.size();
        if (index == size) {
            return true;
        }

        // EOF should also be triggered if the rest of the line is spaces.
        for (unsigned i = index; i < size; ++i) {
            if (!std::isspace(line[i])) {
                return false;
            }
        }

        return true;
    }

    void Tokenizer::operator>>(Token& token) {
        if (eof()) {
            // Reached end of tokenizer.
            token = "";
        }

        unsigned size = line.size();
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

        token = line.substr(index, current - index);
        index = current;
    }



    ShaderInfo::ShaderInfo() : type(ShaderType::INVALID),
                               profile(ShaderProfile::INVALID),
                               version({ .version = -1, .lineNumber = 0 }),
                               success(false)
                               {
    }



    std::unordered_map<ShaderType, ShaderInfo> ShaderPreprocessor::ProcessFile(const std::string& in) {
        std::string filepath = ConvertToNativeSeparators(in);
        std::string extension = ToLower(GetAssetExtension(filepath));

        if (!ValidateShaderExtension(extension)) {
            ImGuiLog::Instance().LogError("ShaderPreprocessor::ProcessFile called on a file with an invalid extension (received '%s', expecting '.vert' for vertex shaders, '.frag' for fragment shaders, '.geom' for geometry shaders, '.tess' for tesselation shaders, '.comp' for compute shaders, or '.glsl' for joint shader files).", extension.c_str());
            throw std::runtime_error("ShaderPreprocessor::ProcessFile call failed - see out/log.txt for more details.");
        }

        if (!Exists(filepath)) {
            ImGuiLog::Instance().LogError("ShaderPreprocessor::ProcessFile called on a file that does not exist ('%s').", filepath.c_str());
            throw std::runtime_error("ShaderPreprocessor::ProcessFile call failed - see out/log.txt for more details.");
        }

        std::unordered_map<ShaderType, ShaderInfo> out;

        // Read file source in.
        std::string source = ReadFile(filepath);

        if (extension == "glsl") {
//            // Processing joint shader file.
//            std::stringstream parser;
//            parser << source;
//
//            std::stringstream file; // Stream to hold the contents of the output file.
//            std::stringstream builder; // For building error messages.
//            Tokenizer tokenizer;
//            Token token;
//            std::string line;
//            unsigned lineNumber = 1;
//            unsigned offset = 0;
//
//            ShaderInfo info { };
//            ProcessingContext context { };
//
//            while (!parser.eof()) {
//                std::getline(parser, line);
//                if (line.empty()) {
//                    file << std::endl;
//                    ++lineNumber;
//                    continue;
//                }
//
//                // First non-empty line (getline ignores lines with only whitespace characters).
//                tokenizer.Set(line);
//                token = tokenizer.Next();
//                offset = 0;
//
//                if (token.data == "#") {
//                    // Spaces are allowed between the '#' and 'type' directives, as long as they are on the same line.
//                    if (!tokenizer.IsValid()) {
//                        // '#' character on a single line is not allowed.
//                        ImGuiLog::Instance().LogError("%s", GetFormattedMessage(context, filepath, line, lineNumber, "Invalid preprocessor directive.", offset + token.before).c_str());
//                        throw std::runtime_error("ShaderPreprocessor::ProcessFile call failed - see out/log.txt for more details.");
//                    }
//                    offset += token.Size(); // Offset starts at the first character of the next token.
//
//                    // Preprocessor directive.
//                    token = tokenizer.Next();
//
//                    if (token.data == "type") {
//                        goto type;
//                    }
//                    else {
//                        goto error;
//                    }
//                }
//                else if (token.data == "#type") {
//                    goto type;
//                }
//                else {
//                    goto error;
//                }
//
//                type: {
//                    if (!tokenizer.IsValid()) {
//                        // #type must have a shader type.
//                        ImGuiLog::Instance().LogError("%s", GetFormattedMessage(context, filepath, line, lineNumber, "#type directive missing shader type.", offset + token.length + 1).c_str());
//                        throw std::runtime_error("ShaderPreprocessor::ProcessFile call failed - see out/log.txt for more details.");
//                    }
//
//                    offset += token.Size();
//
//                    // Shader type.
//                    token = tokenizer.Next();
//                    if (!ValidateShaderType(token.data)) {
//                        ImGuiLog::Instance().LogError("%s", GetFormattedMessage(context, filepath, line, lineNumber, "Invalid shader type (received '" + token.data + "', expecting 'vert' or 'vertex' for vertex shaders, 'frag' or 'fragment' for fragment shaders, 'geom' or 'geometry' for geometry shaders, 'tess' or 'tesselation' for tesselation shaders, 'comp' or 'compute' for compute shaders).", offset + token.length + 1, token.length).c_str());
//                        throw std::runtime_error("ShaderPreprocessor::ProcessFile call failed - see out/log.txt for more details.");
//                    }
//
//                    // Read in shader source up until the next #type directive or eof, whichever is encountered first.
//
//                    continue;
//                }
//
//                error: {
//                    // First token of shader source in a .glsl file must be the #type directive.
//                    ImGuiLog::Instance().LogError("%s", GetFormattedMessage(context, filepath, line, lineNumber, "First token in a .glsl shader file must be a #type preprocessor directive.", offset + token.before).c_str());
//                    throw std::runtime_error("ShaderPreprocessor::ProcessFile call failed - see out/log.txt for more details.");
//                }
//            }
        }
        else {
            // Processing individual shader file.
            ShaderInfo info { };
            info.filepath = filepath;
            info.workingDirectory = GetAssetDirectory(filepath);

            info.type = ToShaderType(extension);
            assert(info.type != ShaderType::INVALID); // Verified above.

            ProcessingContext context { };
            ParseFile(source, info, context);

            std::cout << "warnings" << std::endl;
            for (const std::string& warning : info.warnings) {
                std::cout << warning << std::endl;
            }

            std::cout << "errors" << std::endl;
            for (const std::string& error : info.errors) {
                std::cout << error << std::endl;
            }

            exit(1);

            out.emplace(info.type, std::move(info));
        }

        return out;
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

    ShaderPreprocessor::ShaderPreprocessor() {
    }

    ShaderPreprocessor::~ShaderPreprocessor() {
    }

    bool ShaderPreprocessor::ParseFile(const std::string& source, ShaderInfo& info, ProcessingContext& context) {
        std::string file = context.includeStack.empty() ? info.filepath : context.includeStack.back().file;

        std::stringstream parser;
        parser << source;

        std::stringstream out; // Stream to hold the contents of the output file.
        std::stringstream builder; // For building error messages.

        Tokenizer tokenizer;
        Token token;
        std::string line;
        unsigned lineNumber = 1;
        unsigned offset = 0;

        while (!parser.eof()) {
            std::getline(parser, line);
            if (line.empty()) {
                goto unchanged;
            }

            tokenizer.str(line);
            tokenizer >> token;
            offset = 0;

            if (token.data == "#") {
                // Spaces are allowed between the '#' and 'include' / 'pragma' directives, as long as they are on the same line.
                if (tokenizer.eof()) {
                    // '#' character on a single line is not allowed.
                    info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "Invalid preprocessor directive.", offset + token.before));
                    info.success = false;

                    return false;
                }

                offset += token.size();
                tokenizer >> token; // Preprocessor directive.

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
                if (tokenizer.eof()) {
                    // #include must have filepath token to include.
                    info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "#include directive missing \"FILENAME\" or FILENAME.", offset + token.before + token.data.size() + 1));
                    info.success = false;

                    return false;
                }

                offset += token.size();
                tokenizer >> token; // Filename.

                offset += token.before; // Get to the first character (quotation mark in the case of #include "...").

                // Process include token for correct formatting.
                bool first = (token.data[0] == '\"');
                bool last = (token.data[token.data.size() - 1] == '\"');

                if ((first && !last) || (!first && last)) {
                    // Filepath needs to be either be surrounded by "" or have no decoration.
                    info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "Include filepath must be in the format \"FILENAME\" or FILENAME.", offset, token.data.size()));
                    info.success = false;

                    return false;
                }

                std::string filepath = token.data;
                bool eraseQuotes = first && last;

                if (eraseQuotes) {
                    filepath.erase(std::remove(filepath.begin(), filepath.end(), '\"'), filepath.end());
                }

                filepath = ConvertToNativeSeparators(filepath);

                // Check for extraneous characters after the #include filepath.
                if (!tokenizer.eof()) {
                    // Account for the erased quotation marks from #include "..."
                    //                                                      ^   ^
                    unsigned extraCharactersOffset = offset + filepath.size() + (eraseQuotes ? 2 : 0);
                    Token temp { };

                    // Have error message point to the start of the first extraneous token.
                    tokenizer >> temp;
                    extraCharactersOffset += temp.before;

                    // Skip any spaces before the start of the first extraneous token.
                    unsigned numExtraCharacters = temp.data.size();
                    while (!tokenizer.eof()) {
                        tokenizer >> temp;
                        numExtraCharacters += temp.size();
                    }

                    info.warnings.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "Extraneous tokens at the end of #version directive will be ignored.", extraCharactersOffset, numExtraCharacters));
                }

                if (!Exists(filepath)) {
                    if (GetAssetDirectory(filepath).empty()) {
                        // Asset has no directory - include local to another directory.
                        std::string name = GetAssetName(filepath);
                        std::string extension = GetAssetExtension(filepath);

                        if (extension.empty()) {
                            offset += name.size();

                            if (filepath.find_last_of('.') != std::string::npos) {
                                // Filepath has dot (but no extension), have error message point at the dot.
                                offset += 1;
                            }

                            info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "Invalid file extension for included file.", offset));
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

                            info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset, token.data.size()));
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

                            info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset, token.data.size()));
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

                        info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset, token.data.size()));
                        info.success = false;

                        return false;
                    }
                }

                ShaderInclude include { .parentFile = file, .lineNumber = lineNumber, .file = filepath };

                // Check to make sure base shader file is not being included.
                if (filepath == info.filepath) {
                    // Including base shader file.
                    builder.str("");
                    builder << "Detected circular include of file '" << filepath << "' (cannot include base shader file for processing).";

                    info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber,builder.str(), offset, token.data.size()));
                    info.success = false;

                    return false;
                }

                // Check to make sure no files include themselves.
                // .back() references the most recently included file (still being processed).
                if (!context.includeStack.empty() && filepath == context.includeStack.back().file) {
                    builder.str("");
                    builder << "Detected circular include of file '" << filepath << "' (self inclusion).";

                    info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber,builder.str(), offset, token.data.size()));
                    info.success = false;

                    return false;
                }

                // Circular dependencies are stored in the ProcessingContext and are currently being processed (incomplete).
                for (const ShaderInclude& data : context.includeStack) {
                    if (data == include) {
                        builder.str("");
                        builder << "Detected circular include of file '" << filepath << "' (original include found in file '" << data.parentFile << "' on line " << data.lineNumber << ").";

                        info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber,builder.str(), offset, token.data.size()));
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
                    builder << "Duplicate include of file '" << filepath << "' encountered (original include found in file '" << dependency.parentFile << "' on line " << dependency.lineNumber << ").";

                    info.warnings.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset, token.data.size()));
                }
                else {
                    // Register new "in-flight" dependency.
                    context.includeStack.emplace_back(include);

                    if (!ParseFile(ReadFile(filepath), info, context)) {
                        // Error messages get printed from where the error occurred.
                        return false;
                    }

                    // Update 'info' struct with data from parsed file.
                    info.dependencies.emplace(include);

                    // Remove "in-flight" dependency.
                    context.includeStack.pop_back();
                }

                ++lineNumber;
                continue;
            }

            version: {
                if (tokenizer.eof()) {
                    // #version must have shader version number.
                    info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "#version directive missing version number.", offset + token.before + token.data.size() + 1));
                    info.success = false;

                    return false;
                }

                offset += token.size();
                tokenizer >> token; // Version.

                ShaderVersion version { .version = std::stoi(token.data), .file = file, .lineNumber = lineNumber };

                if (info.version != -1) {
                    // Shader already has versioning information.
                    if (version != info.version) {
                        // Encountered different shader version.
                        builder.str("");
                        builder << "Shader version mismatch, using shader version '" << info.version.version << "' found on line " << info.version.lineNumber << " of file '" << info.version.file << "'.";

                        info.warnings.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset + token.before, token.data.size()));
                    }
                }
                else {
                    // Shader will be the version of the first found #version directive.
                    if (!ValidateShaderVersion(version.version)) {
                        builder.str("");
                        builder << "Invalid shader version (received '" << token.data << "', expecting 110, 120, 130, 140, 150, 330, 400, 410, 420, 430, 440, 450, or 460).";

                        info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset + token.before, token.data.size()));
                        info.success = false;

                        return false;
                    }

                    info.version = version;

                    // Determine which version context to use (optional).
                    if (!tokenizer.eof()) {
                        offset += token.size();
                        tokenizer >> token; // Shader profile (explicitly specified).

                        if (!ValidateShaderProfile(token.data)) {
                            builder.str("");
                            builder << "Invalid shader profile (received '" << token.data << "', expecting 'core' or 'compatibility').";

                            info.errors.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset + token.before, token.data.size()));
                            info.success = false;

                            return false;
                        }

                        info.profile = ToShaderProfile(token.data);
                        offset += token.size();

                        // Check for extraneous characters after the #version directive.
                        if (!tokenizer.eof()) {
                            unsigned extraCharactersOffset = offset;

                            Token temp { };

                            // Have error message point to the start of the first extraneous token.
                            tokenizer >> temp;
                            extraCharactersOffset += temp.before;

                            // Skip any spaces before the start of the first extraneous token.
                            unsigned numExtraCharacters = temp.data.size();
                            while (!tokenizer.eof()) {
                                tokenizer >> temp;
                                numExtraCharacters += temp.size();
                            }

                            info.warnings.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "Extra tokens at the end of #version directive will be ignored.", extraCharactersOffset, numExtraCharacters));
                            out << line.substr(0, offset) << std::endl; // Remove extra characters at the end of line.
                        }
                        else {
                            out << line << std::endl;
                        }
                    }
                    else {
                        // Shader profile not provided, default to ShaderProfile::CORE.

                        // Manual offset: should point to the character directly after the shader version number, regardless of how many additional whitespace characters there are at the end of the line.
                        info.warnings.emplace_back(GetFormattedMessage(context, file, line, lineNumber, "Encountered eol when expecting shader profile - defaulting to 'core'.", offset + token.size() + 1));
                        info.profile = ShaderProfile::CORE;

                        // Ensure proper spacing when appending shader profile to source file.
                        out << line;
                        if (!std::isspace(line[line.size() - 1])) {
                            out << ' ';
                        }
                        out << ToString(info.profile) << std::endl;
                    }
                }

                ++lineNumber;
                continue;
            }

            type: {
                // #type is not a valid preprocessor directive in "regular" shader files.
                // .glsl files should have #type directives, but they get parsed out before the call to this function.
                builder.str("");
                builder << "#type preprocessor directive is not valid in '." << GetAssetExtension(file) << "' files. Line will be ignored."; // Extension is validated above.

                info.warnings.emplace_back(GetFormattedMessage(context, file, line, lineNumber, builder.str(), offset + token.before, token.data.size()));

                out << std::endl;
                ++lineNumber;
                continue;
            }

            unchanged:
            {
                // Type of token is not processed, keep line unchanged.
                out << line << std::endl;
                ++lineNumber;
                continue;
            }
        }

        info.source = out.str();
        info.success = true;
        return true;
    }

    std::string ShaderPreprocessor::GetFormattedMessage(const ProcessingContext& context, const std::string& file, const std::string& line, unsigned lineNumber, const std::string& message, unsigned offset, unsigned length) const {
        static std::stringstream builder;
        builder.str("");

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
            builder << "In file included from " << context.includeStack[0].parentFile << ':' << context.includeStack[0].lineNumber;

            if (size > 1) {
                builder << ',' << std::endl;

                // Align any additional messages to the first line.
                for (int i = 1; i < size - 1; ++i) {
                    builder << "                 from " << context.includeStack[i].parentFile << ':' << context.includeStack[i].lineNumber << ',' << std::endl;
                }
                builder << "                 from " << context.includeStack[size - 1].parentFile << ':' << context.includeStack[size - 1].lineNumber;
            }

            builder << ':' << std::endl;
        }

        builder << file << ':' << lineNumber << ':' << offset + 1 << ": " << message << std::endl;
        builder << ' ' << std::fixed << std::setw(4) << lineNumber << " | " << line << std::endl;
        builder << ' ' << std::fixed << std::setw(4) << ' ' << " | ";

        if (offset > 0) {
            for (unsigned i = 0; i < offset; ++i) {
                builder << ' ';
            }
        }
        builder << '^';

        if (length > 0) {
            for (unsigned i = 0; i < length - 1; ++i) {
                builder << '~';
            }
        }

        builder << std::endl;
        return builder.str();
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

}
