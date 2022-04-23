
#ifndef SANDBOX_TEXTURE_H
#define SANDBOX_TEXTURE_H

#include "pch.h"

namespace Sandbox {

    class Texture {
        public:
            enum AttachmentType {
                COLOR,
                DEPTH,
                UNKNOWN
            };

            explicit Texture(std::string name);
            ~Texture();

            void Bind() const;
            void Unbind() const;

            [[nodiscard]] const std::string& GetName() const;
            [[nodiscard]] GLuint ID() const;
            [[nodiscard]] int GetWidth() const;
            [[nodiscard]] int GetHeight() const;

            void ReserveData(AttachmentType attachmentType, int contentWidth, int contentHeight);
            void ReserveData(const std::string& textureName);

            void WriteDataToDirectory(const std::string& directory) const;

            void SetAttachmentLocation(GLuint attachmentLocation);
            [[nodiscard]] GLuint GetAttachmentLocation() const;
            [[nodiscard]] AttachmentType GetAttachmentType() const;

        private:
            void WriteData(const std::string& filepath, GLenum format, GLenum type, int channels) const;

            int _contentWidth;
            int _contentHeight;

            std::string _name;
            bool _stbLoaded;
            GLuint _textureID;
            AttachmentType _attachmentType;
            GLuint _attachmentLocation;
    };

    typedef std::pair<Texture*, int> TextureSampler;

}

#endif //SANDBOX_TEXTURE_H
