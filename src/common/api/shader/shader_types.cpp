
#include "common/api/shader/shader_types.h"
#include "common/utility/directory.h"

namespace Sandbox {

    std::size_t ShaderDataTypeSize(ShaderDataType shaderDataType) {
        switch (shaderDataType) {
            case ShaderDataType::INT:
            case ShaderDataType::UINT:
                return sizeof(int);
            case ShaderDataType::BOOL:
                return sizeof(bool);
            case ShaderDataType::FLOAT:
                return sizeof(float);
            case ShaderDataType::VEC2:
                return sizeof(glm::vec2);
            case ShaderDataType::IVEC2:
                return sizeof(glm::ivec2);
            case ShaderDataType::VEC3:
                return sizeof(glm::vec3);
            case ShaderDataType::UVEC3:
                return sizeof(glm::uvec3);
            case ShaderDataType::VEC4:
                return sizeof(glm::vec4);
            case ShaderDataType::UVEC4:
                return sizeof(glm::uvec4);
            case ShaderDataType::MAT4:
                return sizeof(glm::mat4);
            default:
                throw std::runtime_error("Unknown shader data type provided to ShaderDataTypeSize.");
        }
    }

//    ShaderType::ShaderType(GLenum type) : openGL_(type) {
//        switch (openGL_) {
//            case GL_VERTEX_SHADER:
//                extension_ = "vert";
//                break;
//            case GL_FRAGMENT_SHADER:
//                extension_ = "frag";
//                break;
//            case GL_GEOMETRY_SHADER:
//                extension_ = "geom";
//                break;
//            case GL_COMPUTE_SHADER:
//                extension_ = "comp";
//                break;
//            default:
//                openGL_ = GL_INVALID_VALUE;
//                extension_ = "";
//                break;
//        }
//    }
//
//    ShaderType::ShaderType(const std::string& filepath) {
//        std::string file = ConvertToNativeSeparators(filepath);
//        std::size_t dotPosition = file.find_last_of('.');
//
//        // Get file extension.
//        std::string extension;
//
//        if (dotPosition != std::string::npos) {
//            extension = file.substr(dotPosition + 1);
//        }
//        else {
//            extension = filepath;
//        }
//
//        bool valid = false;
//
//        // Found extension.
//        extension_ = extension;
//        if (extension_ == "vert") {
//            // Vertex shader.
//            openGL_ = GL_VERTEX_SHADER;
//            valid = true;
//        }
//        else if (extension_ == "frag") {
//            openGL_ = GL_FRAGMENT_SHADER;
//            valid = true;
//        }
//        else if (extension_ == "geom") {
//            openGL_ = GL_GEOMETRY_SHADER;
//            valid = true;
//        }
//        else if (extension_ == "comp") {
//            openGL_ = GL_COMPUTE_SHADER;
//            valid = true;
//        }
//
//        if (!valid) {
//            openGL_ = GL_INVALID_VALUE;
//            extension_ = "";
//        }
//    }
//
//    ShaderType::~ShaderType() = default;
//
//    GLenum ShaderType::ToOpenGLType() const {
//        return openGL_;
//    }
//
//    std::string ShaderType::ToString() const {
//        switch (openGL_) {
//            case GL_VERTEX_SHADER:
//                return "VERTEX";
//            case GL_FRAGMENT_SHADER:
//                return "FRAGMENT";
//            case GL_GEOMETRY_SHADER:
//                return "GEOMETRY";
//            case GL_COMPUTE_SHADER:
//                return "COMPUTE";
//            default:
//                return "INVALID";
//        }
//    }

}

