
#ifndef SANDBOX_TEXTURE_LIBRARY_H
#define SANDBOX_TEXTURE_LIBRARY_H

#include "pch.h"
#include "common/texture/texture.h"

namespace Sandbox {

    class TextureLibrary {
        public:
            TextureLibrary();
            ~TextureLibrary();

            void AddTexture(const std::string& textureName, const std::string& textureFilepath);
            void AddTexture(Texture* texture);

            [[nodiscard]] Texture* GetTexture(const std::string& textureName) const;

        private:
            std::unordered_map<std::string, Texture*> _textures;
    };

}

#endif //SANDBOX_TEXTURE_LIBRARY_H
