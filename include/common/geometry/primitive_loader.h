
#ifndef SANDBOX_PRIMITIVE_LOADER_H
#define SANDBOX_PRIMITIVE_LOADER_H

#include "pch.h"
#include "mesh.h"

namespace Sandbox {

    class PrimitiveLoader {
        public:
            enum class PrimitiveType {
                PLANE
            };

            static PrimitiveLoader& GetInstance();

            Mesh LoadPrimitive(PrimitiveType primitiveType);

        private:
            PrimitiveLoader();
            ~PrimitiveLoader();

            void LoadPlane();

            std::unordered_map<PrimitiveType, Mesh> _primitives;
    };

}

#endif //SANDBOX_PRIMITIVE_LOADER_H
