
#ifndef SANDBOX_VAO_H
#define SANDBOX_VAO_H

#include "pch.h"
#include "common/api/buffer/vbo.h"
#include "common/api/buffer/ebo.h"
#include "common/api/buffer/buffer.h"

namespace Sandbox {

    class VertexArrayObject {
        public:
            VertexArrayObject();
            ~VertexArrayObject();

            VertexArrayObject(const VertexArrayObject& other);
            VertexArrayObject& operator=(const VertexArrayObject& other);

            void Bind() const;
            void Unbind() const;

            // Assumes VAO is bound.
            void AddVBO(const std::string& name, const BufferLayout& bufferLayout);
            [[nodiscard]] VertexBufferObject* GetVBO(const std::string& name);

            // Each VAO has one bound EBO.
            [[nodiscard]] ElementBufferObject* GetEBO();

        private:
            [[nodiscard]] GLenum ConvertShaderDataTypeToOpenGLDataType(ShaderDataType shaderDataType) const;

            unsigned bufferID_;
            unsigned currentAttributeIndex_;

            std::unordered_map<std::string, VertexBufferObject> vbos_;
            ElementBufferObject ebo_;
    };

}

#endif //SANDBOX_VAO_H
