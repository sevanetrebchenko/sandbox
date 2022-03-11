
#include "common/api/shader/shader_descriptor.h"
#include "common/utility/directory.h"
#include "common/utility/log.h"

namespace Sandbox {

	std::string ToString(ShaderProfile profile) {
		switch (profile) {
			case ShaderProfile::CORE:
				return "core";
			case ShaderProfile::COMPATIBILITY:
				return "compatibility";
			case ShaderProfile::INVALID:
			default:
				return "invalid";
		}
	}

    ShaderProfile ToShaderProfile(const std::string& in) {
		std::string profile = ToLower(in);

		if (profile == "core") {
			return ShaderProfile::CORE;
		}
		else if (profile == "compatibility") {
			return ShaderProfile::COMPATIBILITY;
		}
		else {
			return ShaderProfile::INVALID;
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

		if (type == "vert" || type == "vertex") {
			return ShaderType::VERTEX;
		}
		else if (type == "frag" || type == "fragment") {
			return ShaderType::FRAGMENT;
		}
		else if (type == "geom" || type == "geometry") {
			return ShaderType::GEOMETRY;
		}
		else if (type == "tess" || type == "tesselation") {
			return ShaderType::TESSELATION;
		}
		else if (type == "comp" || type == "compute") {
			return ShaderType::COMPUTE;
		}
		else {
			return ShaderType::INVALID;
		}
	}

    ShaderVersion::ShaderVersion(int version, unsigned int lineNumber) : data(version),
                                                                         lineNumber(lineNumber)
                                                                         {
    }

    ShaderVersion::~ShaderVersion() = default;

    bool ShaderVersion::operator==(const ShaderVersion& other) const {
        return data == other.data;
    }

    bool ShaderVersion::operator!=(const ShaderVersion& other) const {
        return !(operator==(other));
    }

    ShaderInclude::ShaderInclude(const std::string& filepath, unsigned lineNumber) : filepath(ConvertToNativeSeparators(filepath)),
                                                                                     lineNumber(lineNumber)
                                                                                     {
    }

    ShaderInclude::~ShaderInclude() = default;

    bool ShaderInclude::operator==(const ShaderInclude& other) const {
        return filepath == other.filepath;
    }

    bool ShaderInclude::operator!=(const ShaderInclude& other) const {
        return !(operator==(other));
    }

}

namespace std {

    std::size_t hash<Sandbox::ShaderInclude>::operator()(const Sandbox::ShaderInclude& data) const {
        return std::hash<std::string>{ }(data.filepath);
    }

}
