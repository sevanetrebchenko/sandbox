
#include "common/api/shader/shader_parser.h"
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    ShaderParser::ShaderParser() {
    }

    ShaderParser::~ShaderParser() {
    }

    ShaderParser::Result ShaderParser::Parse(const std::string& filepath) {
        Result result(filepath);

        return result;
    }

    std::string ShaderParser::GetLine(std::ifstream& stream) const {
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

    void ShaderParser::ThrowFormattedError(const std::string& line, unsigned int lineNumber, const std::string& message, int index) const {
        std::stringstream builder;

        // file:line number:offset: error: message
        builder << path_ + ':' + std::to_string(lineNumber) + ':' + std::to_string(index) + ':' + "error: " + message << std::endl;
        builder << std::fixed << std::setfill(' ') << std::setw(4) << std::to_string(lineNumber) << " | " << line << std::endl;
        builder << "     | " << std::setfill(' ') << std::setw(index) << "^" << std::endl;

        std::string error = builder.str();
        ImGuiLog::Instance().LogWarning(error.c_str());
        throw std::runtime_error(error);
    }

    ShaderParser::Result::Result(const std::string& filepath) : type(ConvertToNativeSeparators(filepath)),
                                                                version(-1),
                                                                success(false)
                                                                {
    }

    ShaderParser::Result::~Result() = default;

    ShaderParser::ShaderInclude::ShaderInclude(const std::string& filename, unsigned lineNumber) : filename(filename),
                                                                                                   lineNumber(lineNumber)
                                                                                                   {
    }

    ShaderParser::ShaderInclude::~ShaderInclude() = default;

    bool ShaderParser::ShaderInclude::operator==(const ShaderParser::ShaderInclude& other) const {
        return filename == other.filename;
    }

    std::size_t ShaderParser::ShaderIncludeHash::operator()(const ShaderParser::ShaderInclude& data) const noexcept {
        return std::hash<std::string>{}(data.filename);
    }

}
