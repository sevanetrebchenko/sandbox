
#ifndef SANDBOX_TEXTURE_LIBRARY_H
#define SANDBOX_TEXTURE_LIBRARY_H

#include <sandbox_pch.h>
#include <framework/texture.h>

namespace Sandbox {

    class TextureLibrary {
        public:
            static TextureLibrary& GetInstance();

            void AddTexture(const std::string& textureName, const std::string& textureFilepath);
            void AddTexture(Texture* texture);

            [[nodiscard]] Texture* GetTexture(const std::string& textureName) const;

        private:
            TextureLibrary();
            ~TextureLibrary();

            std::unordered_map<std::string, Texture*> _textures;
    };

}

#endif //SANDBOX_TEXTURE_LIBRARY_H
