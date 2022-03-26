
#pragma once

#include "pch.h"
#include "common/api/buffer/vao.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class VAOManager : public ISingleton<VAOManager> {
        public:
            REGISTER_SINGLETON(VAOManager);

            [[nodiscard]] VertexArrayObject* GetVAO(const std::string& filepath);

        private:
            VAOManager();
            ~VAOManager() override;

            std::unordered_map<std::string, VertexArrayObject*> vaos_;
    };


}