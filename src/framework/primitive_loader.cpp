
#include <framework/primitive_loader.h>

namespace Sandbox {


    PrimitiveLoader &PrimitiveLoader::GetInstance() {
        static PrimitiveLoader instance;
        return instance;
    }

    Mesh PrimitiveLoader::LoadPrimitive(PrimitiveLoader::PrimitiveType primitiveType) {
        auto primitiveIter = _primitives.find(primitiveType);
        if (primitiveIter != _primitives.end()) {
            return primitiveIter->second;
        }

        switch (primitiveType) {
            case PrimitiveType::PLANE:
                LoadPlane();
        }

        return _primitives[primitiveType];
    }

    PrimitiveLoader::PrimitiveLoader() {

    }

    PrimitiveLoader::~PrimitiveLoader() {

    }

    void PrimitiveLoader::LoadPlane() {
        Mesh mesh(GL_TRIANGLES);

        std::vector<glm::vec3> vertices;
        std::vector<unsigned> indices;
        std::vector<glm::vec2> uv;

        vertices.emplace_back(-1.0f, 1.0f, 0.0f);
        vertices.emplace_back(-1.0f, -1.0f, 0.0f);
        vertices.emplace_back(1.0f, -1.0f, 0.0f);
        vertices.emplace_back(1.0f, 1.0f, 0.0f);

        indices.emplace_back(0);
        indices.emplace_back(1);
        indices.emplace_back(2);
        indices.emplace_back(0);
        indices.emplace_back(2);
        indices.emplace_back(3);

        uv.emplace_back(0.0f, 1.0f);
        uv.emplace_back(0.0f, 0.0f);
        uv.emplace_back(1.0f, 0.0f);
        uv.emplace_back(1.0f, 1.0f);


        mesh.SetVertices(vertices);
        mesh.SetIndices(indices);
        mesh.SetUV(uv);
        mesh.RecalculateNormals();
        _primitives[PrimitiveType::PLANE] = mesh;
    }
}
