
#ifndef SANDBOX_TEXTURE_LIBRARY_H
#define SANDBOX_TEXTURE_LIBRARY_H

#include <sandbox.h>
#include <framework/texture.h>
#include <framework/singleton.h>

namespace Sandbox {

    class TextureLibrary : public Singleton<TextureLibrary> {
        public:
            REGISTER_SINGLETON(TextureLibrary);

            void Initialize() override;
            void Shutdown() override;

            void AddTexture(const std::string& textureName, const std::string& textureFilepath);
            void AddTexture(Texture* texture);

            [[nodiscard]] Texture* GetTexture(const std::string& textureName) const;

        private:
            TextureLibrary();
            ~TextureLibrary() override;

            std::unordered_map<std::string, Texture*> _textures;
    };

}

#endif //SANDBOX_TEXTURE_LIBRARY_H
