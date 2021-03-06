
#include <scenes/ascii_post_processing/character_bitmap.h>

namespace Sandbox {

    CharacterBitmap::CharacterBitmap(int characterWidth, int characterHeight) : _isDirty(true),
                                                                                _characterWidth(characterWidth),
                                                                                _characterHeight(characterHeight)
                                                                                {
        // Allocate bitmap memory.
        int numDesiredBits = characterWidth * characterHeight;
        if (numDesiredBits > MAX_NUM_BITMAPS * 32) { // 32 bits in an unsigned int.
            throw std::out_of_range("Character size is too big.");
        }

        int size = (numDesiredBits / 32) + (bool)(numDesiredBits % 32);
        _bitmap.resize(size);
    }

    CharacterBitmap::~CharacterBitmap() {

    }

    void CharacterBitmap::SetBit(unsigned position) {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size()) {
            throw std::out_of_range("Bit position out of range of character size (SetBit).");
        }

        _bitmap[elementPosition] |= (1 << bitPosition);
        _isDirty = true;
    }

    void CharacterBitmap::ToggleBit(unsigned position) {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size()) {
            throw std::out_of_range("Bit position out of range of character size (ToggleBit).");
        }

        _bitmap[elementPosition] ^= (1 << bitPosition);
        _isDirty = true;
    }

    void CharacterBitmap::ClearBit(unsigned position) {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size()) {
            throw std::out_of_range("Bit position out of range of character size (ClearBit).");
        }

        _bitmap[elementPosition] &= ~(1 << bitPosition);
        _isDirty = true;
    }

    unsigned CharacterBitmap::GetNumIndices() const {
        return _bitmap.size();
    }

    unsigned CharacterBitmap::GetValueAtIndex(unsigned index) const {
        if (index >= _bitmap.size()) {
            throw std::out_of_range("Bit position out of range of character size (GetBitsAtIndex).");
        }

        return _bitmap[index];
    }

    bool CharacterBitmap::IsDirty() const {
        return _isDirty;
    }

    void CharacterBitmap::Clean() {
        _isDirty = false;
    }

    unsigned CharacterBitmap::GetWidth() const {
        return _characterWidth;
    }

    unsigned CharacterBitmap::GetHeight() const {
        return _characterHeight;
    }

}
