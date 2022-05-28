
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
			case ShaderType::TESSELATION_CONTROL:
				return "tesselation control";
            case ShaderType::TESSELATION_EVALUATION:
                return "tesselation evaluation";
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
			return ShaderType::TESSELATION_CONTROL; // TODO:
		}
		else if (type == "comp" || type == "compute") {
			return ShaderType::COMPUTE;
		}
		else {
			return ShaderType::INVALID;
		}
	}



    FileInfo::FileInfo() : lineNumber(0) {
    }

    FileInfo::FileInfo(std::string file, unsigned int lineNumber) : file(std::move(file)),
                                                                    lineNumber(lineNumber)
                                                                    {
    }



    ShaderVersion::ShaderVersion() : location(),
                                     version(-1)
                                     {
    }

    ShaderVersion::ShaderVersion(FileInfo location, int version) : location(std::move(location)),
                                                                   version(version)
                                                                   {
    }

    ShaderVersion::ShaderVersion(std::string file, unsigned int lineNumber, int version) : location(std::move(file), lineNumber),
                                                                                           version(version)
                                                                                           {
    }

    bool ShaderVersion::operator==(const ShaderVersion& other) const {
        return version == other.version;
    }

    bool ShaderVersion::operator!=(const ShaderVersion& other) const {
        return !(operator==(other));
    }

    ShaderVersion::operator int() const {
        return version;
    }




    ShaderInclude::ShaderInclude() = default;

    ShaderInclude::ShaderInclude(FileInfo parent, std::string file) : parent(std::move(parent)),
                                                                      file(std::move(file))
                                                                      {
    }

    ShaderInclude::ShaderInclude(std::string parentFile, unsigned int lineNumber, std::string file) : parent(std::move(parentFile), lineNumber),
                                                                                                      file(std::move(file))
                                                                                                      {
    }

    bool ShaderInclude::operator==(const ShaderInclude& other) const {
        return file == other.file;
    }

    bool ShaderInclude::operator!=(const ShaderInclude& other) const {
        return !(operator==(other));
    }

}

namespace std {

    std::size_t hash<Sandbox::ShaderInclude>::operator()(const Sandbox::ShaderInclude& data) const {
        return std::hash<std::string>{ }(data.file);
    }

}
