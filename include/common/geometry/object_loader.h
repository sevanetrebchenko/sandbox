
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include "pch.h"
#include "mesh.h"

namespace Sandbox {

    class OBJLoader {
        public:
            struct Request {
                explicit Request(std::string filepath);
                ~Request();

                std::string filepath_;
            };

            static OBJLoader& Instance();
            Mesh LoadFromFile(const Request& request);

        private:
            OBJLoader();
            ~OBJLoader();

            std::unordered_map<std::string, Mesh> meshes_;
    };

}

#endif //SANDBOX_OBJECT_LOADER_H
