
#include <framework/buffer/vao.h>

namespace Sandbox {

    VertexArrayObject::VertexArrayObject() : _bufferID(0),
                                             _currentAttributeIndex(0),
                                             _ebo(nullptr) {
        glGenVertexArrays(1, &_bufferID);
    }

    VertexArrayObject::~VertexArrayObject() {
        ClearVBOs();
        glDeleteVertexArrays(1, &_bufferID);
    }

    void VertexArrayObject::Bind() const {
        // VBOs/EBO get bound on initialization.
        glBindVertexArray(_bufferID);

//        for (VertexBufferObject* vbo : _vbos) {
//            if (vbo) {
//                vbo->Bind();
//            }
//        }

        if (_ebo) {
            _ebo->Bind();
        }
    }

    void VertexArrayObject::Unbind() const {
        for (VertexBufferObject* vbo : _vbos) {
            if (vbo) {
                vbo->Unbind();
            }
        }

        if (_ebo) {
            _ebo->Unbind();
        }

        glBindVertexArray(0);
    }

    void VertexArrayObject::AddVBO(VertexBufferObject *vertexBufferObject) {
        if (!vertexBufferObject->GetBufferLayout().GetBufferElements().empty()) {
            Bind();
            vertexBufferObject->Bind();

            const BufferLayout& vertexBufferLayout = vertexBufferObject->GetBufferLayout();

            for (auto& vertexBufferElement : vertexBufferLayout.GetBufferElements()) {
                unsigned elementCount = vertexBufferElement.GetComponentCount();
                switch (vertexBufferElement.GetShaderDataType()) {
                    case ShaderDataType::BOOL:
                    case ShaderDataType::INT:
                    case ShaderDataType::FLOAT:
                    case ShaderDataType::VEC2:
                    case ShaderDataType::VEC3:
                    case ShaderDataType::VEC4:
                        glEnableVertexAttribArray(_currentAttributeIndex);
                        glVertexAttribPointer(_currentAttributeIndex,
                                              vertexBufferElement.GetComponentCount(),
                                              ConvertShaderDataTypeToOpenGLDataType(vertexBufferElement.GetShaderDataType()),
                                              GL_FALSE,
                                              vertexBufferLayout.GetStride(),
                                              (void*)vertexBufferElement.GetBufferOffset());
                        ++_currentAttributeIndex;
                        break;
                    case ShaderDataType::MAT4:
                        for (unsigned i = 0; i < elementCount; ++i) {
                            glEnableVertexAttribArray(_currentAttributeIndex);
                            glVertexAttribPointer(_currentAttributeIndex,
                                                  vertexBufferElement.GetComponentCount(),
                                                  ConvertShaderDataTypeToOpenGLDataType(vertexBufferElement.GetShaderDataType()),
                                                  GL_FALSE,
                                                  vertexBufferLayout.GetStride(),
                                                  (void*)(vertexBufferElement.GetBufferOffset() + sizeof(float) * elementCount * i));
                            ++_currentAttributeIndex;
                        }
                        break;
                    default:
                        throw std::runtime_error("Unknown shader data type provided to VertexArrayObject::SetVBO.");
                }
            }

            Unbind();
            vertexBufferObject->Unbind();

            _vbos.push_back(vertexBufferObject);
        }
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

    void VertexArrayObject::SetEBO(ElementBufferObject *elementBufferObject) {
        _ebo = elementBufferObject;

        // Restore state to ensure ebo binding to correct VAO.
        Bind();
        _ebo->Bind();
    }

    ElementBufferObject *VertexArrayObject::GetEBO() const {
        return _ebo;
    }

    void VertexArrayObject::ClearVBOs() {
        // VBOs/EBO get bound on initialization.
        glBindVertexArray(_bufferID);

        for (VertexBufferObject* vbo : _vbos) {
            if (vbo) {
                vbo->Unbind();
                delete vbo;
            }
        }

        _vbos.clear();

        if (_ebo) {
            _ebo->Unbind();
            delete _ebo;
        }

        _currentAttributeIndex = 0;
    }

}