
#include "common/api/buffer/vao_manager.h"

namespace Sandbox {

    VAOManager& VAOManager::Instance() {
        static VAOManager instance;
        return instance;
    }

    VertexArrayObject* VAOManager::GetVAO(const std::string& filepath) {
        auto iterator = vaos_.find(filepath);
        if (iterator == vaos_.end()) {
            VertexArrayObject* vao = new VertexArrayObject();

            vao->Bind();

            // Vertex position.
            {
                BufferLayout bufferLayout { };
                bufferLayout.SetBufferElements( { BufferElement { ShaderDataType::VEC3, "vertexPosition" } } );
                vao->AddVBO("position", bufferLayout);
            }

            // Vertex normal.
            {
                BufferLayout bufferLayout { };
                bufferLayout.SetBufferElements( { BufferElement { ShaderDataType::VEC3, "vertexNormal" } } );
                vao->AddVBO("normal", bufferLayout);
            }

            // Vertex UV coordinates.
            {
                BufferLayout bufferLayout { };
                bufferLayout.SetBufferElements( { BufferElement { ShaderDataType::VEC2, "vertexUV" } } );
                vao->AddVBO("uv", bufferLayout);
            }

            // TODO: TBN frame, skinned models.
            vao->Unbind();
            vaos_.emplace(filepath, vao);
            return vaos_[filepath];
        }
        else {
            return iterator->second;
        }
    }

    VAOManager::VAOManager() {
    }

    VAOManager::~VAOManager() {
    }
}
