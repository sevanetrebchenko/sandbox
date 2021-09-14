
#include <framework/object_loader.h>

namespace Sandbox {

    void OBJLoader::Initialize() {
    }

    void OBJLoader::Shutdown() {
    }

    OBJLoader::OBJLoader() {
    }

    OBJLoader::~OBJLoader() {
    }

    void OBJLoader::MinMaxVertex(const glm::vec3 &vertex, glm::vec3 &minimum, glm::vec3 &maximum) const {
        // Find mesh extrema.
        if (vertex.x < minimum.x) {
            minimum.x = vertex.x;
        }
        else if (vertex.x > maximum.x) {
            maximum.x = vertex.x;
        }

        if (vertex.y < minimum.y) {
            minimum.y = vertex.y;
        }
        else if (vertex.y > maximum.y) {
            maximum.y = vertex.y;
        }

        if (vertex.z < minimum.z) {
            minimum.z = vertex.z;
        }
        else if (vertex.z > maximum.z) {
            maximum.z = vertex.z;
        }
    }

    Mesh OBJLoader::LoadFromFile(const std::string &objFilePath) {
        if (_loadedMeshes.find(objFilePath) != _loadedMeshes.end()) {
            return _loadedMeshes[objFilePath]; // Make copy.
        }

        // Loading new mesh.
        Mesh mesh(GL_TRIANGLES);
        std::vector<glm::vec3> vertices;
        std::vector<unsigned> indices;
        std::unordered_map<glm::vec3, unsigned> uniqueVertices;
        std::vector<glm::vec2> uv;

        glm::vec3 minimum(std::numeric_limits<float>::max());
        glm::vec3 maximum(std::numeric_limits<float>::lowest());

        // Prepare tinyobj loading parameters.
        tinyobj::attrib_t attributes; // Holds all positions, normals, and texture coordinates.
        std::vector<tinyobj::shape_t> shapeData; // Holds all separate objects and their faces.
        std::vector<tinyobj::material_t> materialData;
        std::string warning;
        std::string error;

        // Triangulation enabled by default.
        if (!tinyobj::LoadObj(&attributes, &shapeData, &materialData, &warning, &error, objFilePath.c_str())) {
            throw std::runtime_error("Failed to load OBJ file: " + objFilePath + ". Provided information: " + warning + " (WARNING) " + error + "(ERROR)");
        }

        bool hasVertexNormals = !attributes.normals.empty();
        bool hasTextureCoordinates = !attributes.texcoords.empty();
        bool hasVertexColors = !attributes.colors.empty();

        // Push shape data.
        for (const tinyobj::shape_t& shape : shapeData) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {
                int vertexBase = 3 * index.vertex_index;
                glm::vec3 vertex(attributes.vertices[vertexBase + 0], attributes.vertices[vertexBase + 1], attributes.vertices[vertexBase + 2]);

                int vertexNormalBase = 3 * index.normal_index;
                glm::vec3 vertexNormal;
                if (hasVertexNormals) {
                    vertexNormal = glm::vec3(attributes.normals[vertexNormalBase + 0], attributes.normals[vertexNormalBase + 1], attributes.normals[vertexNormalBase + 2]);
                }

                int textureCoordinateBase = 2 * index.texcoord_index;
                glm::vec2 textureCoordinate;
                if (hasTextureCoordinates) {
                    textureCoordinate = glm::vec2(attributes.texcoords[textureCoordinateBase + 0], attributes.texcoords[textureCoordinateBase + 1]);
                }

                int vertexColorBase = vertexBase;
                glm::vec3 vertexColor;
                if (hasVertexColors) {
                    vertexColor = glm::vec3(attributes.colors[vertexColorBase + 0], attributes.colors[vertexColorBase + 1], attributes.colors[vertexColorBase + 2]);
                }

                // Found unique vertex.
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = vertices.size();
                    vertices.push_back(vertex);
                    MinMaxVertex(vertex, minimum, maximum);

                    if (hasTextureCoordinates) {
                        uv.push_back(textureCoordinate);
                    }
                }

                // In case of duplicate vertex, push back index instead.
                indices.push_back(uniqueVertices[vertex]);
            }
        }

        // Normalize mesh.
        TransformToOrigin(vertices, minimum, maximum);
        ScaleToUniform(vertices, minimum, maximum);

        mesh.SetVertices(vertices);
        mesh.SetIndices(indices);
        mesh.SetUV(uv);
        mesh.RecalculateNormals();
        _loadedMeshes[objFilePath] = mesh;
        return mesh;
    }

    void OBJLoader::TransformToOrigin(std::vector<glm::vec3> &vertices, const glm::vec3 &minimum, const glm::vec3 &maximum) const {
        // Center model at (0, 0, 0)
        glm::vec3 centerPosition = glm::vec3((minimum + maximum) / 2.0f);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), -centerPosition);
        for (glm::vec3& vertex : vertices) {
            vertex = transform * glm::vec4(vertex, 1.0f);
        }
    }

    void OBJLoader::ScaleToUniform(std::vector<glm::vec3> &vertices, const glm::vec3 &minimum, const glm::vec3 &maximum) const {
        glm::vec3 boundingBoxDimensions = maximum - minimum;

        // Scale the mesh to range [-1 1] on all axes.
        float maxDimension = std::max(boundingBoxDimensions.x, std::max(boundingBoxDimensions.y, boundingBoxDimensions.z));
        glm::mat4 uniformScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / maxDimension));

        for (glm::vec3& vertex : vertices) {
            vertex = uniformScale * glm::vec4(vertex, 1.0f);
        }
    }

}
