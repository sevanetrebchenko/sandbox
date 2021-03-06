
#ifndef SANDBOX_ASCII_CHARACTER_MAP_H
#define SANDBOX_ASCII_CHARACTER_MAP_H

#include <sandbox_pch.h>
#include <scenes/ascii_post_processing/character_bitmap.h>
#include <framework/buffer/ubo.h>
#include <scenes/ascii_post_processing/ascii_defines.h>

namespace Sandbox {

    class AsciiCharacterMap {
        public:
            AsciiCharacterMap(unsigned characterWidth, unsigned characterHeight);
            ~AsciiCharacterMap();

            void UpdateData(UniformBufferObject* ubo);

            // Characters must be added IN ORDER from darkest to lightest.
            void AddCharacter(const CharacterBitmap& characterBitmap);

        private:
            unsigned _characterWidth;
            unsigned _characterHeight;

            bool _isDirty;
            std::vector<CharacterBitmap> _fontsheet;
            UniformBlock _uniformBlock;
    };

}

#endif //SANDBOX_ASCII_CHARACTER_MAP_H
