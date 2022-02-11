
#include "common/api/buffer/vao.h"

namespace Sandbox {

    VertexArrayObject::VertexArrayObject() : bufferID_(0),
                                             currentAttributeIndex_(0)
                                             {
        glGenVertexArrays(1, &bufferID_);

        // Bind EBO.
        Bind();
        ebo_.Bind();
    }

    VertexArrayObject::~VertexArrayObject() {
        glDeleteVertexArrays(1, &bufferID_);
    }

    VertexArrayObject::VertexArrayObject(const VertexArrayObject& other) : bufferID_(0),
                                                                           currentAttributeIndex_(0)
    {
        glGenVertexArrays(1, &bufferID_);
        initialized = false;

        Bind();

        ebo_.Bind();

        // Deep copy VBO layouts.
        for (const std::pair<const std::string, VertexBufferObject>& vboData : other.vbos_) {
            const std::string& vboName = vboData.first;
            const VertexBufferObject& vbo = vboData.second;

            AddVBO(vboName, vbo.GetBufferLayout());
        }

        Unbind();
    }

    VertexArrayObject& VertexArrayObject::operator=(const VertexArrayObject& other) {
        if (this == &other) {
            return *this;
        }

        bufferID_ = 0;
        currentAttributeIndex_ = 0;
        initialized = false;

        // Clear currently attached VBOs.
        vbos_.clear();

        Bind();

        ebo_.Bind();

        // Deep copy VBO layouts.
        for (const std::pair<const std::string, VertexBufferObject>& vboData : other.vbos_) {
            const std::string& vboName = vboData.first;
            const VertexBufferObject& vbo = vboData.second;

            AddVBO(vboName, vbo.GetBufferLayout());
        }

        Unbind();

        return *this;
    }

    void VertexArrayObject::Bind() const {
        // VBOs/EBO get bound on initialization.
        glBindVertexArray(bufferID_);
    }

    void VertexArrayObject::Unbind() const {
        glBindVertexArray(0);
    }

    void VertexArrayObject::AddVBO(const std::string& name, const BufferLayout& bufferLayout) {
        if (bufferLayout.GetBufferElements().empty()) {
            return;
        }

        if (vbos_.find(name) != vbos_.end()) {
            // VBO with provided name already exists.
            assert(false); // TODO:
        }

        // Create new VBO.
        vbos_.emplace(name, bufferLayout);
        VertexBufferObject& vbo = vbos_.at(name);

        vbo.Bind();

        for (auto& vertexBufferElement : bufferLayout.GetBufferElements()) {
            unsigned elementCount = vertexBufferElement.GetComponentCount();
            switch (vertexBufferElement.GetShaderDataType()) {
                case ShaderDataType::BOOL:
                case ShaderDataType::INT:
                case ShaderDataType::FLOAT:
                case ShaderDataType::VEC2:
                case ShaderDataType::VEC3:
                case ShaderDataType::VEC4:
                    glEnableVertexAttribArray(currentAttributeIndex_);
                    glVertexAttribPointer(currentAttributeIndex_,
                                          vertexBufferElement.GetComponentCount(),
                                          ConvertShaderDataTypeToOpenGLDataType(vertexBufferElement.GetShaderDataType()),
                                          GL_FALSE,
                                          bufferLayout.GetStride(),
                                          (void*)vertexBufferElement.GetBufferOffset());
                    ++currentAttributeIndex_;
                    break;
                case ShaderDataType::MAT4:
                    for (unsigned i = 0; i < elementCount; ++i) {
                        glEnableVertexAttribArray(currentAttributeIndex_);
                        glVertexAttribPointer(currentAttributeIndex_,
                                              vertexBufferElement.GetComponentCount(),
                                              ConvertShaderDataTypeToOpenGLDataType(vertexBufferElement.GetShaderDataType()),
                                              GL_FALSE,
                                              bufferLayout.GetStride(),
                                              (void*)(vertexBufferElement.GetBufferOffset() + sizeof(float) * elementCount * i));
                        ++currentAttributeIndex_;
                    }
                    break;
                default:
                    throw std::runtime_error("Unknown shader data type provided to VertexArrayObject::SetVBO.");
            }
        }

        vbo.Unbind();
    }

    GLenum VertexArrayObject::ConvertShaderDataTypeToOpenGLDataType(ShaderDataType shaderDataType) const {
        switch (shaderDataType) {
            case ShaderDataType::BOOL:
                return GL_BOOL;
            case ShaderDataType::INT:
                return GL_INT;
            case ShaderDataType::FLOAT:
            case ShaderDataType::VEC2:
            case ShaderDataType::VEC3:
            case ShaderDataType::VEC4:
            case ShaderDataType::MAT4:
                return GL_FLOAT;
            default:
                throw std::runtime_error("Unknown shader data type provided to VertexArrayObject::ConvertShaderDataTypeToOpenGLDataType.");
        }
    }

    ElementBufferObject* VertexArrayObject::GetEBO() {
        return &ebo_;
    }

    VertexBufferObject* VertexArrayObject::GetVBO(const std::string& name) {
        auto iterator = vbos_.find(name);
        if (iterator != vbos_.end()) {
            return &iterator->second;
        }
        else {
            return nullptr;
        }

    }

}