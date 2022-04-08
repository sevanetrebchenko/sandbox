
#include "common/texture/texture.h"
#include "common/utility/directory.h"
#include "common/utility/log.h"

namespace Sandbox {

    Texture::Texture(std::string name) : _name(std::move(name)),
                                         _contentWidth(-1),
                                         _contentHeight(-1),
                                         _stbLoaded(false) {
        glGenTextures(1, &_textureID);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &_textureID);
    }

    void Texture::Bind() const {
        glBindTexture(GL_TEXTURE_2D, _textureID);
    }

    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    const std::string &Texture::GetName() const {
        return _name;
    }

    GLuint Texture::ID() const {
        return _textureID;
    }

    int Texture::GetWidth() const {
        return _contentWidth;
    }

    int Texture::GetHeight() const {
        return _contentHeight;
    }

    void Texture::ReserveData(AttachmentType attachmentType, int contentWidth, int contentHeight) {
        _stbLoaded = false;
        _attachmentType = attachmentType;

        Bind();

        _contentWidth = contentWidth;
        _contentHeight = contentHeight;

        switch (_attachmentType) {
            case COLOR:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, contentWidth, contentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                break;
            case DEPTH:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, contentWidth, contentHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
                break;
            case UNKNOWN:
                throw std::runtime_error("Usage of attachment type UNKNOWN is reserved.");
        }

        // Texture wrapping.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Texturing filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Unbind();
    }

    void Texture::ReserveData(const std::string &textureName) {
        _stbLoaded = true;

        stbi_set_flip_vertically_on_load(false);

        std::string name = ConvertToNativeSeparators(textureName);
        std::string extension = GetAssetExtension(textureName);

        int width, height, channels;

        Bind();

        if (extension == "png" || extension == "jpg") {
            unsigned char* data = stbi_load(name.c_str(), &width, &height, &channels, 0);
            if (!data) {
                throw std::runtime_error("Failed to load texture: " + textureName);
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else if (extension == "hdr") {
            // High definition range image.
            float* data = stbi_loadf(name.c_str(), &width, &height, &channels, 0);
            if (!data) {
                throw std::runtime_error("Failed to load texture: " + textureName);
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
            stbi_image_free(data);
        }

        // Texture wrapping.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Texturing filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Unbind();
    }

    void Texture::SetAttachmentLocation(GLuint attachmentLocation) {
        _attachmentLocation = attachmentLocation;
    }

    GLuint Texture::GetAttachmentLocation() const {
        if (_stbLoaded) {
            throw std::runtime_error("Getting attachment location of texture allocated with texture data (STB).");
        }

        return _attachmentLocation;
    }

    Texture::AttachmentType Texture::GetAttachmentType() const {
        if (_stbLoaded) {
            throw std::runtime_error("Getting attachment location of texture allocated with texture data (STB).");
        }

        return _attachmentType;
    }

    void Texture::WriteDataToDirectory(const std::string &directory) const {
        ImGuiLog& log = ImGuiLog::Instance();

        // Make sure directory exists.
        if (!std::filesystem::is_directory(directory)) {
            log.LogTrace("Creating new directory under: %s", directory.c_str());
            std::filesystem::create_directory(directory);
        }

        // Convert texture name to be directory friendly.
        std::stringstream stringbuilder;
        for (char character : _name) {
            // Convert spaces to underscores.
            if (character == ' ') {
                stringbuilder << '_';
            }
            else {
                stringbuilder << static_cast<char>(std::tolower(character));
            }
        }

        std::string filepath = directory + stringbuilder.str() + ".png";

        switch (_attachmentType) {
            case COLOR:
                WriteData(filepath, GL_RGBA, GL_UNSIGNED_BYTE, 4);
                log.LogTrace("Saving RGBA image file: \"%s\" under location: %s", _name.c_str(), filepath.c_str());
                break;
            case DEPTH:
                WriteData(filepath, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 1);
                log.LogTrace("Saving DEPTH image file: \"%s\" under location: %s", _name.c_str(), filepath.c_str());
                break;
            case UNKNOWN:
                log.LogError("Attempting to write screenshot of existing texture. No image data generated.");
                break;
        }
    }

    void Texture::WriteData(const std::string& filepath, GLenum format, GLenum type, int channels) const {
        unsigned char* textureData = new unsigned char[_contentWidth * _contentHeight * channels];

        // Get texture data to store inside textureData array.
        Bind();
        glGetTexImage(GL_TEXTURE_2D, 0, format, type, textureData);
        Unbind();

        stbi_flip_vertically_on_write(true);
        stbi_write_png(filepath.c_str(), _contentWidth, _contentHeight, channels, textureData, 0);

        delete[] textureData;
    }

    void Texture::SetData(int contentWidth, int contentHeight, const std::vector<unsigned char>& data) {
        Bind();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, contentWidth, contentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

        // Texture wrapping.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Texturing filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Unbind();
    }

    void Texture::SetData(int contentWidth, int contentHeight, const std::vector<float>& data) {
        Bind();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, contentWidth, contentHeight, 0, GL_RGB, GL_FLOAT, data.data());

        // Texture wrapping.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Texturing filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Unbind();
    }

}