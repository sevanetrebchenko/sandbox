
#include <framework/object_loader.h>
#include <framework/directory_utils.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    OBJLoader::LoadingParameters::LoadingParameters(std::string filename, bool normalizePositions, bool normalizeScale) : filename(std::move(filename)),
                                                                                                                          normalizePositions(normalizePositions),
                                                                                                                          normalizeScale(normalizeScale)
                                                                                                                          {
    }

    OBJLoader::OBJLoader() {

    }

    Mesh OBJLoader::LoadFromFile(std::string objFilePath, bool normalizePositions, bool normalizeScale) {
        objFilePath = NativePathConverter::ConvertToNativeSeparators(objFilePath);

        if (_loadedMeshes.find(objFilePath) != _loadedMeshes.end()) {
            return _loadedMeshes[objFilePath]; // Make copy.
        }

        std::ifstream fileReader;

        // Open the file.
        fileReader.open(objFilePath);

        if (fileReader.is_open()) {
            // Successfully opened the file.
            Mesh mesh(GL_TRIANGLES);
            std::vector<glm::vec3> vertices;
            std::vector<unsigned> indices;
            glm::vec3 minimumVertex = glm::vec3(std::numeric_limits<float>::max());
            glm::vec3 maximumVertex = glm::vec3(std::numeric_limits<float>::lowest());

            glm::vec3 currentVertex;
            std::vector<unsigned> currentFaceIndices;

            std::string currentLine;

            while (!fileReader.eof()) {
                // Get the next full line.
                std::string currentString;
                std::getline(fileReader, currentString);

                // Create a stream to parse each line into tokens.
                std::stringstream stringParser(currentString);

                // Get the first token (OBJ tag)
                std::string token;
                stringParser >> token;

                if (token.size() == 1) {
                    switch (token[0]) {
                        // Vertex position (3D).
                        case 'v':
                            currentVertex = ReadVertexPosition(stringParser);
                            MinMaxVertex(currentVertex, minimumVertex, maximumVertex);
                            vertices.emplace_back();
                            break;

                        // Face.
                        case 'f':
                            // Get all face indices broken into triangles (in the case of more than 3 indices)
                            currentFaceIndices = ReadFace(stringParser);

                            // Append indices into mesh.
                            for (int i = 0; i < currentFaceIndices.size(); i += 3) {
                                // Track indices.
                                indices.emplace_back(currentFaceIndices[i]);
                                indices.emplace_back(currentFaceIndices[i + 1]);
                                indices.emplace_back(currentFaceIndices[i + 2]);
                            }
                            break;

                            // Group specifier.
                        case 'g':
                            break;
                    }
                } else if (token.size() == 2) {
                    // Vertex attribute.
                    if (token[0] == 'v') {
                        switch (token[1]) {
                            // Vertex normal (3D).
                            case 'n':
                                // Unimplemented (yet).
                                break;

                            // Texture coordinate (2D).
                            case 't':
                                // Unimplemented (yet).
                                break;
                        }
                    }
                } else {
                    // Nothing yet.
                }
            }

            // Get the center of the mesh.
            glm::vec3 centerPosition = glm::vec3((minimumVertex + maximumVertex) / 2.0f);
            glm::mat4 translationToZero = glm::translate(glm::mat4(1.0f), -centerPosition);

            glm::vec3 boundingBoxDimensions = maximumVertex - minimumVertex;

            // Scale the mesh to [-1 1] by the largest dimension to keep model aspect ratio.
            float maxDimension = std::max(boundingBoxDimensions.x, std::max(boundingBoxDimensions.y, boundingBoxDimensions.z));
            glm::mat4 uniformScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / maxDimension));

            // Apply default transform to each vertex to scale to range [-1, 1] centered at (0, 0, 0) if the user wants a normalized mesh.
            if (normalizePositions && normalizeScale) {
                glm::mat4 transform = uniformScale * translationToZero;

                for (glm::vec3& vertex : vertices) {
                    vertex = transform * glm::vec4(vertex, 1.0f);
                }
            }
            else {
                // Normalize positions (transform the mesh to be centered at (0, 0, 0)).
                if (normalizePositions) {
                    for (glm::vec3& vertex : vertices) {
                        vertex = translationToZero * glm::vec4(vertex, 1.0f);
                    }
                }
                // Normalize scale (transform the mesh to span the range [-1, 1]).
                if (normalizeScale) {
                    glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.0001f));
                    for (glm::vec3& vertex : vertices) {
                        vertex = transform * glm::vec4(vertex, 1.0f);
                    }
                }
            }

            // ImGuiLog::GetInstance().LogTrace("Successfully loaded object: %s", objFilePath.c_str());

            _loadedMeshes.emplace(objFilePath, mesh);
            return mesh;
        }
        else {
            // Could not open file.
            throw std::runtime_error("Could not open .obj file: \"" + objFilePath + "\"");
        }
    }

    Mesh OBJLoader::LoadFromFile(const OBJLoader::LoadingParameters &loadingParameters) {
        return LoadFromFile(loadingParameters.filename, loadingParameters.normalizePositions, loadingParameters.normalizeScale);
    }

    glm::vec3 OBJLoader::ReadVertexPosition(std::stringstream &stringParser) const {
        std::string token;
        glm::vec3 position;
        unsigned index = 0;

        while (stringParser >> token) {
            position[index++] = std::stof(token);
        }

        return position;
    }

    std::vector<unsigned> OBJLoader::ReadFace(std::stringstream &stringParser) const {
        std::string token;
        static std::vector<unsigned> indices;
        unsigned lastIndex = 0;

        // Read the first 3 as normal (each face has at least 3 indices)
        for (int i = 0; i < 3; ++i) {
            stringParser >> token;

            // Token contains a /, need to parse it.
            if (token.find('/') != std::string::npos) {
                // Get the next full line.
                std::string number;
                std::stringstream tokenReader(token);

                std::getline(tokenReader, number, '/');
                lastIndex = std::stoi(number) - 1;
            }
                // Parse the token as normal.
            else {
                lastIndex = std::stoi(token) - 1;
            }

            indices.emplace_back(lastIndex); // Face indices in .obj files start with 1, not 0.
        }

        // Read additional vertices as triangle fan indices.
        while (stringParser >> token) {
            int currentIndex = std::stoi(token) - 1;  // Face indices in .obj files start with 1, not 0.

            indices.emplace_back(indices[0]);
            indices.emplace_back(lastIndex);
            indices.emplace_back(currentIndex);

            lastIndex = currentIndex;
        }

        return std::move(indices);
    }

//    Mesh OBJLoader::LoadPlane() {
//        static Mesh planePrefab;
//        static bool created = false;
//
//        if (created) {
//            return planePrefab;
//        }
//
//        planePrefab.vertices.emplace_back(-1.0f, 1.0f, 0.0f);
//        planePrefab.vertices.emplace_back(-1.0f, -1.0f, 0.0f);
//        planePrefab.vertices.emplace_back(1.0f, -1.0f, 0.0f);
//        planePrefab.vertices.emplace_back(1.0f, 1.0f, 0.0f);
//
//        planePrefab.triangles.emplace_back(0);
//        planePrefab.triangles.emplace_back(1);
//        planePrefab.triangles.emplace_back(2);
//        planePrefab.triangles.emplace_back(0);
//        planePrefab.triangles.emplace_back(2);
//        planePrefab.triangles.emplace_back(3);
//
//        planePrefab.uv.emplace_back(0.0f, 1.0f);
//        planePrefab.uv.emplace_back(0.0f, 0.0f);
//        planePrefab.uv.emplace_back(1.0f, 0.0f);
//        planePrefab.uv.emplace_back(1.0f, 1.0f);
//
//        planePrefab.RecalculateNormals();
//        created = true;
//
//        return planePrefab;
//    }

    OBJLoader::~OBJLoader() {
    }

    OBJLoader &OBJLoader::GetInstance() {
        static OBJLoader instance;
        return instance;
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

//    Mesh OBJLoader::LoadSphere() {
//        static bool created = false;
//        static Mesh sphereMesh;
//
//        static int numHorizontalDivisions = 40;
//        static int numVerticalDivisions = 40;
//
//        if (created) {
//            return sphereMesh;
//        }
//        BoundingVolume boundingVolume;
//        float vx;
//        float vy;
//        float vz;
//
//        float sectorStep = 2.0f * PI / (float) numHorizontalDivisions;
//        float stackStep = PI / numVerticalDivisions;
//        float sectorAngle;
//        float stackAngle;
//
//        // Generate vertices.
//        for (int i = 0; i <= numVerticalDivisions; ++i) {
//            stackAngle = PI / 2 - ((float) i * stackStep);
//            vz = std::sin(stackAngle);
//
//            for (int j = 0; j <= numHorizontalDivisions; ++j) {
//                sectorAngle = (float) j * sectorStep;
//
//                vx = std::cos(stackAngle) * std::cos(sectorAngle);
//                vy = std::cos(stackAngle) * std::sin(sectorAngle);
//
//                // Record vertex.
//                glm::vec3 vertex = glm::vec3(vx, vy, vz);
//                sphereMesh.vertices.emplace_back(vertex);
//                boundingVolume.AddVertex(vertex);
//            }
//        }
//
//        // Generate indices
//        unsigned currentStack;
//        unsigned nextStack;
//        for (int i = 0; i < numVerticalDivisions; ++i) {
//            currentStack = i * (numHorizontalDivisions + 1);
//            nextStack = currentStack + numHorizontalDivisions + 1;
//
//            for (int j = 0; j < numHorizontalDivisions; ++j) {
//                // First and last faces are made out of triangles, not quads
//                if (i != 0) {
//                    sphereMesh.triangles.emplace_back(currentStack);
//                    sphereMesh.triangles.emplace_back(nextStack);
//                    sphereMesh.triangles.emplace_back(currentStack + 1);
//                }
//
//                // Break up quad into two triangles.
//                if (i != (numVerticalDivisions - 1)) {
//                    sphereMesh.triangles.emplace_back(currentStack + 1);
//                    sphereMesh.triangles.emplace_back(nextStack);
//                    sphereMesh.triangles.emplace_back(nextStack + 1);
//                }
//
//                ++currentStack;
//                ++nextStack;
//            }
//        }
//
//        // Apply default transform to each vertex to scale to range [-1, 1] centered at (0, 0, 0).
//        glm::mat4 defaultTransform = boundingVolume.GetDefaultTransform();
//        for (glm::vec3& vertex : sphereMesh.vertices) {
//            vertex = defaultTransform * glm::vec4(vertex, 1.0f);
//        }
//
//        sphereMesh.RecalculateNormals();
//        created = true;
//        return sphereMesh;
//    }

}
