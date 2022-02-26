
#ifndef SANDBOX_OBJECT_LOADER_H
#define SANDBOX_OBJECT_LOADER_H

#include "pch.h"
#include "common/geometry/mesh.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class OBJLoader : public Singleton<OBJLoader> {
        public:
            REGISTER_SINGLETON(OBJLoader);

            struct Request {
                explicit Request(std::string filepath);
                ~Request();

                std::string filepath_;
            };

            Mesh LoadFromFile(const Request& request);

        private:
            OBJLoader();
            ~OBJLoader();

            std::unordered_map<std::string, Mesh> meshes_;
    };

}

#endif //SANDBOX_OBJECT_LOADER_H
