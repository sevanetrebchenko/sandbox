
#ifndef SANDBOX_CHARACTER_BITMAP_H
#define SANDBOX_CHARACTER_BITMAP_H

#include <sandbox_pch.h>
#include <scenes/ascii_post_processing/ascii_defines.h>

namespace Sandbox {

    class CharacterBitmap {
        public:
            CharacterBitmap(std::string name, unsigned characterWidth, unsigned characterHeight);
            CharacterBitmap(const CharacterBitmap& other);
            CharacterBitmap(CharacterBitmap&&) noexcept;
            ~CharacterBitmap();

            [[nodiscard]] bool IsDirty() const;

            void SetBit(unsigned bitPosition);
            void ToggleBit(unsigned bitPosition);
            void ClearBit(unsigned bitPosition);

            [[nodiscard]] unsigned GetNumIndices() const;
            [[nodiscard]] unsigned GetValueAtIndex(unsigned index) const;
            [[nodiscard]] unsigned GetWidth() const;
            [[nodiscard]] unsigned GetHeight() const;
            [[nodiscard]] float GetCoverage() const;

        private:
            friend class AsciiCharacterMap;
            friend class nlohmann::adl_serializer<Sandbox::CharacterBitmap>;

            [[nodiscard]] bool TestBit(unsigned bitPosition) const;
            void Clean();

            bool _isDirty;
            unsigned _numSetBits;
            unsigned _characterWidth;
            unsigned _characterHeight;
            std::string _name;
            std::vector<unsigned> _bitmap;
    };

}

#include <framework/serialization/character_bitmap_serialization.tpp>

#endif //SANDBOX_CHARACTER_BITMAP_H
