
#include <framework/texture_library.h>
#include <framework/directory_utils.h>

namespace Sandbox {

    TextureLibrary::TextureLibrary() {

    }

    TextureLibrary::~TextureLibrary() {

    }

    void TextureLibrary::AddTexture(const std::string& textureName, const std::string &textureFilepath) {
        Texture* texture = new Texture(textureName);
        texture->Bind();
        texture->ReserveData(NativePathConverter::ConvertToNativeSeparators(textureFilepath));
        texture->Unbind();

        _textures.emplace(textureName, texture);
    }

    void TextureLibrary::AddTexture(Texture *texture) {
        _textures.emplace(texture->GetName(), texture);
    }

    Texture *TextureLibrary::GetTexture(const std::string &textureName) const {
        auto textureIter = _textures.find(textureName);

        if (textureIter != _textures.end()) {
            return textureIter->second;
        }

        return nullptr;
    }

}
