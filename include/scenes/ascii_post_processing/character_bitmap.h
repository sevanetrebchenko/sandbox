
#ifndef SANDBOX_CHARACTER_BITMAP_H
#define SANDBOX_CHARACTER_BITMAP_H

#include <sandbox_pch.h>
#include <scenes/ascii_post_processing/ascii_defines.h>

namespace Sandbox {

    class CharacterBitmap {
        public:
            CharacterBitmap(int characterWidth, int characterHeight);
            ~CharacterBitmap();

            [[nodiscard]] bool IsDirty() const;

            void SetBit(unsigned bitPosition);
            void ToggleBit(unsigned bitPosition);
            void ClearBit(unsigned bitPosition);

            [[nodiscard]] unsigned GetNumIndices() const;
            [[nodiscard]] unsigned GetValueAtIndex(unsigned index) const;
            [[nodiscard]] unsigned GetWidth() const;
            [[nodiscard]] unsigned GetHeight() const;

        private:
            friend class AsciiCharacterMap;
            void Clean();

            bool _isDirty;
            unsigned _characterWidth;
            unsigned _characterHeight;
            std::vector<unsigned> _bitmap;
    };

}

#endif //SANDBOX_CHARACTER_BITMAP_H
