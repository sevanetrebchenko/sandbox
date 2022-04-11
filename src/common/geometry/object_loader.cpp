
#include "common/geometry/object_loader.h"
#include "common/api/buffer/vao_manager.h"

namespace Sandbox {

    OBJLoader::OBJLoader() {
    }

    OBJLoader::~OBJLoader() {
    }

    Mesh OBJLoader::LoadFromFile(const Request& request) {
        const std::string& filename = request.filepath_;

        if (meshes_.find(filename) != meshes_.end()) {
            return meshes_.at(filename); // Make copy.
        }

        // Loading new mesh.
        std::unordered_map<glm::vec3, unsigned> uniqueVertices;
        std::vector<glm::vec3> vertices;
        std::vector<unsigned> indices;
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
        if (!tinyobj::LoadObj(&attributes, &shapeData, &materialData, &warning, &error, filename.c_str())) {
            throw std::runtime_error("Failed to load OBJ file: " + filename + ". Provided information: " + warning + " (WARNING) " + error + "(ERROR)");
        }

        bool hasTextureCoordinates = !attributes.texcoords.empty();

        // Push shape data.
        for (const tinyobj::shape_t& shape : shapeData) {
            for (const tinyobj::index_t& index : shape.mesh.indices) {
                int vertexBase = 3 * index.vertex_index;

                glm::vec3 vertex(attributes.vertices[vertexBase + 0],
                                 attributes.vertices[vertexBase + 1],
                                 attributes.vertices[vertexBase + 2]);

                // min-max vertex to determine original dimensions of mesh.
                minimum = glm::min(vertex, minimum);
                maximum = glm::max(vertex, maximum);

                int textureCoordinateBase = 2 * index.texcoord_index;
                glm::vec2 textureCoordinate(-1.0f);
                if (hasTextureCoordinates) {
                    textureCoordinate = glm::vec2(attributes.texcoords[textureCoordinateBase + 0],
                                                  attributes.texcoords[textureCoordinateBase + 1]);
                }

                if (uniqueVertices.count(vertex) == 0) {
                    // Found new vertex.
                    uniqueVertices[vertex] = vertices.size();
                    vertices.emplace_back(vertex);

                    if (hasTextureCoordinates) {
                        uv.emplace_back(textureCoordinate);
                    }
                }

                indices.emplace_back(uniqueVertices[vertex]);
            }
        }

        // Normalize mesh.
        // Translate vertices to be centered at the origin.
        glm::vec3 centerPosition = glm::vec3((minimum + maximum) / 2.0f);

        // Scale mesh vertices to be on the range [-1, 1] on all axes.
        glm::vec3 boundingBoxDimensions = maximum - minimum;
        float maxDimension = std::max(boundingBoxDimensions.x, std::max(boundingBoxDimensions.y, boundingBoxDimensions.z));

        glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / maxDimension)) * glm::translate(glm::mat4(1.0f), -centerPosition);
        for (glm::vec3& vertex : vertices) {
            vertex = transform * glm::vec4(vertex, 1.0f);
        }

        Mesh mesh { VAOManager::Instance().GetVAO(filename) };
        mesh.SetVertices(vertices);
        mesh.SetIndices(indices, MeshTopology::TRIANGLES);
        mesh.SetUVs(uv);
        mesh.RecalculateNormals();

        // Save mesh for future use.
        meshes_.emplace(filename, mesh);
        return mesh;
    }

    Mesh OBJLoader::LoadSphere() {
        if (meshes_.find("uv sphere") != meshes_.end()) {
            return meshes_.at("uv sphere"); // Make copy.
        }

        const int numHorizontalDivisions = 20;
        const int numVerticalDivisions = 20;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned> indices;

        float sectorStep = 2.0f * glm::pi<float>() / static_cast<float>(numHorizontalDivisions);
        float stackStep = glm::pi<float>() / static_cast<float>(numVerticalDivisions);

        // Generate vertices.
        for (int i = 0; i <= numVerticalDivisions; ++i) {
            float stackAngle = glm::pi<float>() / 2.0f - (static_cast<float>(i) * stackStep);
            float z = std::sin(stackAngle);

            for (int j = 0; j <= numHorizontalDivisions; ++j) {
                float sectorAngle = static_cast<float>(j) * sectorStep;

                float x = std::cos(stackAngle) * std::cos(sectorAngle);
                float y = std::cos(stackAngle) * std::sin(sectorAngle);

                // Record vertex.
                glm::vec3 vertex = glm::vec3(x, y, z);
                vertices.emplace_back(vertex);
                normals.emplace_back(glm::normalize(vertex));
            }
        }

        // Generate indices
        for (int i = 0; i <= numVerticalDivisions; ++i) {
            int currentStack = i * (numHorizontalDivisions + 1);
            int nextStack = currentStack + numHorizontalDivisions + 1;

            for (int j = 0; j <= numHorizontalDivisions; ++j) {
                // First and last faces are made out of triangles, not quads
                if (i != 0) {
                    indices.emplace_back(currentStack);
                    indices.emplace_back(nextStack);
                    indices.emplace_back(currentStack + 1);
                }

                // Break up quad into two triangles.
                if (i != (numVerticalDivisions - 1)) {
                    indices.emplace_back(currentStack + 1);
                    indices.emplace_back(nextStack);
                    indices.emplace_back(nextStack + 1);
                }

                ++currentStack;
                ++nextStack;
            }
        }

        // Remove duplicates.
        Mesh mesh { VAOManager::Instance().GetVAO("uv sphere") };
        mesh.SetVertices(vertices);
        mesh.SetNormals(normals);
        mesh.SetIndices(indices, MeshTopology::TRIANGLES);
        // mesh.RecalculateNormals(); // TODO: recalculate without any duplicate data.

        // Save mesh for future use.
        meshes_.emplace("uv sphere", mesh);
        return mesh;
    }

    OBJLoader::Request::Request(std::string filepath) : filepath_(std::move(filepath)) {
    }

    OBJLoader::Request::~Request() {
    }

}
