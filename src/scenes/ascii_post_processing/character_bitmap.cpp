
#include <scenes/ascii_post_processing/character_bitmap.h>

namespace Sandbox {

    CharacterBitmap::CharacterBitmap(std::string name, unsigned characterWidth, unsigned characterHeight) : _isDirty(true),
                                                                                                            _numSetBits(0),
                                                                                                            _characterWidth(characterWidth),
                                                                                                            _characterHeight(characterHeight),
                                                                                                            _name(std::move(name))
                                                                                                            {
        // Allocate bitmap memory.
        unsigned numDesiredBits = characterWidth * characterHeight;
        if (numDesiredBits > MAX_NUM_BITMAPS * 32) { // 32 bits in an unsigned int.
            throw std::out_of_range("Character size is too big.");
        }

        unsigned size = (numDesiredBits / 32) + (bool)(numDesiredBits % 32);
        _bitmap.resize(size);
    }

    CharacterBitmap::CharacterBitmap(const CharacterBitmap &other) : _isDirty(other._isDirty),
                                                                     _characterWidth(other._characterWidth),
                                                                     _characterHeight(other._characterHeight),
                                                                     _name(other._name),
                                                                     _bitmap(other._bitmap)
                                                                     {
    }

    CharacterBitmap::CharacterBitmap(CharacterBitmap&& other) noexcept : _isDirty(other._isDirty),
                                                                         _characterWidth(other._characterWidth),
                                                                         _characterHeight(other._characterHeight),
                                                                         _name(std::move(other._name)),
                                                                         _bitmap(std::move(other._bitmap))
                                                                         {
    }

    CharacterBitmap::~CharacterBitmap() {

    }

    void CharacterBitmap::SetBit(unsigned position) {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size() * 32) {
            throw std::out_of_range("Bit position out of range of character size (SetBit).");
        }

        // If bit is already set, don't update numSetBits.
        if (!TestBit(bitPosition)) {
            ++_numSetBits;
        }

        _bitmap[elementPosition] |= (1 << bitPosition);
        _isDirty = true;
    }

    void CharacterBitmap::ToggleBit(unsigned position) {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size() * 32) {
            throw std::out_of_range("Bit position out of range of character size (ToggleBit).");
        }

        if (TestBit(bitPosition)) {
            --_numSetBits;
        }
        else {
            ++_numSetBits;
        }

        _bitmap[elementPosition] ^= (1 << bitPosition);
        _isDirty = true;
    }

    void CharacterBitmap::ClearBit(unsigned position) {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size() * 32) {
            throw std::out_of_range("Bit position out of range of character size (ClearBit).");
        }

        // If bit is not set, don't decrement numSetBits.
        if (TestBit(bitPosition)) {
            --_numSetBits;
        }

        _bitmap[elementPosition] &= ~(1 << bitPosition);
        _isDirty = true;
    }

    unsigned CharacterBitmap::GetNumIndices() const {
        return _bitmap.size();
    }

    unsigned CharacterBitmap::GetValueAtIndex(unsigned index) const {
        if (index >= _bitmap.size() * 32) {
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

    float CharacterBitmap::GetCoverage() const {
        float bitAddition = 1.0f / (float)(_characterWidth * _characterHeight);
        return bitAddition * (float)_numSetBits;
    }

    bool CharacterBitmap::TestBit(unsigned int position) const {
        unsigned elementPosition = position / 32;
        unsigned bitPosition = position % 32;

        if (elementPosition >= _bitmap.size() * 32) {
            throw std::out_of_range("Bit position out of range of character size (ClearBit).");
        }

        return _bitmap[elementPosition] & (1 << (bitPosition));
    }

}
