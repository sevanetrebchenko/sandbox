
#ifndef SANDBOX_ASCII_CHARACTER_MAP_H
#define SANDBOX_ASCII_CHARACTER_MAP_H

#include <sandbox.h>
#include <scenes/ascii_post_processing/character_bitmap.h>
#include <framework/buffer/ubo.h>
#include <scenes/ascii_post_processing/ascii_defines.h>

namespace Sandbox {

    class AsciiCharacterMap {
        public:
            explicit AsciiCharacterMap(const std::string& fontsheetPath);
            AsciiCharacterMap(unsigned characterWidth, unsigned characterHeight);
            ~AsciiCharacterMap();

            void UpdateData();

            void AddCharacter(const CharacterBitmap& characterBitmap);

        private:
            void ParseFont(const std::string& filepath);
            void ConstructUniformBlock();

            unsigned _characterWidth;
            unsigned _characterHeight;

            bool _isDirty;
            std::vector<CharacterBitmap> _fontsheet;
            UniformBufferObject _characterUBO;
    };

}

#endif //SANDBOX_ASCII_CHARACTER_MAP_H
