
#include <scenes/ascii_post_processing/ascii_character_map.h>

namespace Sandbox {

    AsciiCharacterMap::AsciiCharacterMap(unsigned characterWidth, unsigned characterHeight) : _characterWidth(characterWidth),
                                                                                              _characterHeight(characterHeight),
                                                                                              _uniformBlock(UniformBlockLayout(2, MAX_NUM_BITMAPS / 4), 1),
                                                                                              _isDirty(true) {
        ConstructUniformBlock();
    }

    AsciiCharacterMap::AsciiCharacterMap(const std::string &filepath) : _uniformBlock(UniformBlockLayout(2, MAX_NUM_BITMAPS / 4), 1),
                                                                        _isDirty(true) {
        ParseFont(NativePathConverter::ConvertToNativeSeparators(filepath));
        ConstructUniformBlock();
    }

    AsciiCharacterMap::~AsciiCharacterMap() {

    }

    void AsciiCharacterMap::AddCharacter(const CharacterBitmap &characterBitmap) {
        // Verify dimensions.
        if (characterBitmap.GetWidth() != _characterWidth || characterBitmap.GetHeight() != _characterHeight) {
            throw std::runtime_error("Dimension mismatch in AddCharacter.");
        }

        if (_fontsheet.size() < MAX_NUM_CHARACTERS) {
            _fontsheet.push_back(characterBitmap);
        }
    }

    void AsciiCharacterMap::UpdateData(UniformBufferObject* ubo) {
        if (_isDirty) {
            ubo->AddUniformBlock(_uniformBlock);
            _isDirty = false;
        }

        // Set UBO data.
        ubo->Bind();

        UniformBlockLayout* characterBufferLayout = _uniformBlock.GetUniformBlockLayout();
        std::vector<UniformBufferElement>& characterBufferElements = characterBufferLayout->GetBufferElements();

        // fontScale
        {
            UniformBufferElement& fontHorizontalScale = characterBufferElements[0];
            unsigned elementOffset = fontHorizontalScale.GetBufferOffset();
            unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(fontHorizontalScale.GetShaderDataType());

            glm::ivec2 fontScale(_characterWidth, _characterHeight);

            ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&fontScale));
        }

        // charactersInUse
        {
            UniformBufferElement& charactersInUse = characterBufferElements[1];
            unsigned elementOffset = charactersInUse.GetBufferOffset();
            unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(charactersInUse.GetShaderDataType());
            std::size_t data = _fontsheet.size(); // TODO: Temporary.
            ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&data));
        }

        unsigned offset = characterBufferLayout->GetInitialOffsetInElements();

        for (int i = 0; i < _fontsheet.size(); ++i) {
            CharacterBitmap& characterBitmap = _fontsheet[i];

            if (characterBitmap.IsDirty()) {
                unsigned bufferElementIndex = offset + characterBufferLayout->GetIntermediateOffsetInElements() * i;

                unsigned numEvenDivisions = characterBitmap.GetNumIndices() / 4;
                unsigned numRemaining = characterBitmap.GetNumIndices() % 4;

                for (int j = 0; j < numEvenDivisions; j += 4) {
                    UniformBufferElement& element = characterBufferElements[bufferElementIndex++];
                    unsigned elementOffset = element.GetBufferOffset();
                    unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());

                    glm::uvec4 data (characterBitmap.GetValueAtIndex(j), characterBitmap.GetValueAtIndex(j + 1), characterBitmap.GetValueAtIndex(j + 2), characterBitmap.GetValueAtIndex(j + 3));
                    ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&data));
                }

                if (numRemaining) {
                    // Fill last element (remaining number of indices).
                    UniformBufferElement& element = characterBufferElements[bufferElementIndex];
                    unsigned elementOffset = element.GetBufferOffset();
                    unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());

                    glm::uvec4 data (0.0f);

                    for (int j = 0; j < numRemaining; ++j) {
                        data[j] = characterBitmap.GetValueAtIndex(numEvenDivisions * 4 + j); // Guaranteed to be less than a full step.
                    }

                    ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&data));
                }

                characterBitmap.Clean();
            }
        }

        ubo->Unbind();
    }

    void AsciiCharacterMap::ParseFont(const std::string &filepath) {
        std::ifstream filestream;
        filestream.open(filepath);

        std::string storage;

        if (filestream.is_open()) {
            unsigned characterWidth;
            unsigned characterHeight;
            filestream >> characterWidth >> characterHeight;
            _characterWidth = characterWidth;
            _characterHeight = characterHeight;

            std::getline(filestream, storage); // Clear NL.

            while (!filestream.eof()) {
                // Pattern goes as follows:
                // Name
                // Bit positions that need to be set.

                // Name.
                std::string name;
                filestream >> name;
                std::getline(filestream, storage); // Clear NL.

                CharacterBitmap character(name, characterWidth, characterHeight);

                // Bit positions that need to be set.
                std::string bitPositions;
                std::getline(filestream, bitPositions);

                std::stringstream bitPositionParser(bitPositions);
                while (!bitPositionParser.eof()) {
                    unsigned bitPosition;
                    bitPositionParser >> bitPosition;

                    character.SetBit(bitPosition);
                }

                _fontsheet.push_back(character);
            }
        }
    }

    void AsciiCharacterMap::ConstructUniformBlock() {
        // Construct block layout.
        UniformBlockLayout* blockLayout = _uniformBlock.GetUniformBlockLayout();

        std::vector<UniformBufferElement> elementList;

        // Global data.
        elementList.emplace_back(ShaderDataType::IVEC2, "fontScale");
        elementList.emplace_back(ShaderDataType::UVEC3, "charactersInUse");

        // Character data.
        for (int character = 0; character < MAX_NUM_CHARACTERS; ++character) {
            std::string characterString = "character" + std::to_string(character);
            for (int bitmap = 0; bitmap < MAX_NUM_BITMAPS / 4; ++bitmap) {
                std::string bitmapString = "bitmap" + std::to_string(bitmap);

                elementList.emplace_back(ShaderDataType::UVEC4, characterString + bitmapString); // std140 rules - every element in an array has an alignment equal to a vec4.
            }
        }

        blockLayout->SetBufferElements(elementList);
    }

}
