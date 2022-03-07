
#include "common/api/shader/shader_element.h"
#include "common/utility/directory.h"

namespace Sandbox {

	std::string ToString(ShaderContext profile) {
		switch (profile) {
			case ShaderContext::CORE:
				return "core";
			case ShaderContext::COMPATIBILITY:
				return "compatibility";
			case ShaderContext::INVALID:
			default:
				return "invalid";
		}
	}

	ShaderContext ToShaderProfile(const std::string& in) {
		std::string profile = ToLower(in);

		if (profile == "core") {
			return ShaderContext::CORE;
		}
		else if (profile == "compatibility") {
			return ShaderContext::COMPATIBILITY;
		}
		else {
			return ShaderContext::INVALID;
		}
	}

	std::string ToString(ShaderType type) {
		switch (type) {
			case ShaderType::VERTEX:
				return "vertex";
			case ShaderType::FRAGMENT:
				return "fragment";
			case ShaderType::GEOMETRY:
				return "geometry";
			case ShaderType::TESSELATION:
				return "tesselation";
			case ShaderType::COMPUTE:
				return "compute";
			case ShaderType::INVALID:
			default:
				return "invalid";
		}
	}

	ShaderType ToShaderType(const std::string& in) {
		std::string type = ToLower(in);

		if (type == "vertex") {
			return ShaderType::VERTEX;
		}
		else if (type == "fragment") {
			return ShaderType::FRAGMENT;
		}
		else if (type == "geometry") {
			return ShaderType::GEOMETRY;
		}
		else if (type == "tesselation") {
			return ShaderType::TESSELATION;
		}
		else if (type == "compute") {
			return ShaderType::COMPUTE;
		}
		else {
			return ShaderType::INVALID;
		}
	}

    ShaderInclude::ShaderInclude(const std::string& filename, unsigned lineNumber) : filename(filename),
                                                                                     lineNumber(lineNumber)
                                                                                     {
    }

    ShaderInclude::~ShaderInclude() = default;

    bool ShaderInclude::operator==(const ShaderInclude& other) const {
        return filename == other.filename;
    }

}
