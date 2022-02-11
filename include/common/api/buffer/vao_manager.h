
#pragma once

#include "pch.h"
#include "common/api/buffer/vao.h"

namespace Sandbox {

    class VAOManager {
        public:
            static VAOManager& Instance();

            [[nodiscard]] VertexArrayObject* GetVAO(const std::string& filepath);

        private:
            VAOManager();
            ~VAOManager();

            std::unordered_map<std::string, VertexArrayObject*> vaos_;
    };


}