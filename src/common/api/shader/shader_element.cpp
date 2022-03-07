
#include "common/api/shader/shader_element.h"

namespace Sandbox {

    ShaderInclude::ShaderInclude(const std::string& filename, unsigned lineNumber) : filename(filename),
                                                                                     lineNumber(lineNumber)
                                                                                     {
    }

    ShaderInclude::~ShaderInclude() = default;

    bool ShaderInclude::operator==(const ShaderInclude& other) const {
        return filename == other.filename;
    }

}
