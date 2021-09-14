
#ifndef SANDBOX_VAO_H
#define SANDBOX_VAO_H

#include <sandbox.h>
#include <framework/buffer/vbo.h>
#include <framework/buffer/ebo.h>
#include <framework/buffer/buffer.h>

namespace Sandbox {

    class VertexArrayObject {
        public:
            VertexArrayObject();
            ~VertexArrayObject();

            void Bind() const;
            void Unbind() const;

            void AddVBO(VertexBufferObject* vertexBufferObject);
            void ClearVBOs();
            void SetEBO(ElementBufferObject* elementBufferObject);
            [[nodiscard]] ElementBufferObject* GetEBO() const;

        private:
            [[nodiscard]] GLenum ConvertShaderDataTypeToOpenGLDataType(ShaderDataType shaderDataType) const;

            unsigned _bufferID;
            unsigned _currentAttributeIndex;
            std::vector<VertexBufferObject*> _vbos;
            ElementBufferObject* _ebo;
    };

}

#endif //SANDBOX_VAO_H
