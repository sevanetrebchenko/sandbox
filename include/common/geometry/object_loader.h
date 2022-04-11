
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include "pch.h"
#include "common/geometry/mesh.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class OBJLoader : public ISingleton<OBJLoader> {
        public:
            REGISTER_SINGLETON(OBJLoader);

            struct Request {
                explicit Request(std::string filepath);
                ~Request();

                std::string filepath_;
            };

            Mesh LoadFromFile(const Request& request);

            // Loads UV sphere.
            Mesh LoadSphere(); // TODO: Abstract.

        private:
            OBJLoader();
            ~OBJLoader();

            std::unordered_map<std::string, Mesh> meshes_;
    };

}

#endif //SANDBOX_OBJECT_LOADER_H
