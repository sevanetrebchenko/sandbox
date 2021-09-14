
#include <scenes/ascii_post_processing/ascii_character_map.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    AsciiCharacterMap::AsciiCharacterMap(unsigned characterWidth, unsigned characterHeight) : _characterWidth(characterWidth),
                                                                                              _characterHeight(characterHeight),
                                                                                              _isDirty(true) {
        ConstructUniformBlock();
    }

    AsciiCharacterMap::AsciiCharacterMap(const std::string &filepath) : _isDirty(true) {
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

        ImGuiLog* log = Singleton<ImGuiLog>::GetInstance();

        if (_fontsheet.size() < MAX_NUM_CHARACTERS) {
            float characterCoverage = characterBitmap.GetCoverage();
            bool foundLocation = false;

            log->LogTrace("Adding character bitmap with name: '%s' (Coverage: %i%).", characterBitmap.GetName().c_str(), (int)(characterCoverage * 100));

            for (auto characterIter = _fontsheet.cbegin(); characterIter != _fontsheet.cend(); ++characterIter) {
                const CharacterBitmap& character = *characterIter;

                if (character.GetCoverage() < characterCoverage) {
                    // Push front.
                    if (characterIter == _fontsheet.cbegin()) {
                        _fontsheet.insert(_fontsheet.cbegin(), characterBitmap);
                    }
                    // Insert before the one with greater coverage.
                    else {
                        _fontsheet.insert(characterIter - 1, characterBitmap);
                    }

                    foundLocation = true;
                    break;
                }
            }

            if (!foundLocation) {
                _fontsheet.push_back(characterBitmap);
            }

            _isDirty = true;
        }
    }

    void AsciiCharacterMap::UpdateData() {
        if (_isDirty) {
            // Set UBO data.
            _characterUBO.Bind();

            UniformBlockLayout& characterBufferLayout = _characterUBO.GetUniformBlock().GetUniformBlockLayout();
            const std::vector<UniformBufferElement>& characterBufferElements = characterBufferLayout.GetBufferElements();

            // fontScale
            {
                const UniformBufferElement& fontHorizontalScale = characterBufferElements[0];
                unsigned elementOffset = fontHorizontalScale.GetBufferOffset();
                unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(fontHorizontalScale.GetShaderDataType());

                glm::ivec2 fontScale(_characterWidth, _characterHeight);

                _characterUBO.SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&fontScale));
            }

            // charactersInUse
            {
                const UniformBufferElement& charactersInUse = characterBufferElements[1];
                unsigned elementOffset = charactersInUse.GetBufferOffset();
                unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(charactersInUse.GetShaderDataType());
                unsigned data = _fontsheet.size();
                _characterUBO.SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&data));
            }

            unsigned offset = characterBufferLayout.GetInitialOffsetInElements();

            for (int i = 0; i < _fontsheet.size(); ++i) {
                CharacterBitmap& characterBitmap = _fontsheet[i];

                if (characterBitmap.IsDirty()) {
                    unsigned bufferElementIndex = offset + characterBufferLayout.GetIntermediateOffsetInElements() * i;

                    unsigned numEvenDivisions = characterBitmap.GetNumIndices() / 4;
                    unsigned numRemaining = characterBitmap.GetNumIndices() % 4;

                    for (int j = 0; j < numEvenDivisions; j += 4) {
                        const UniformBufferElement& element = characterBufferElements[bufferElementIndex++];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());

                        glm::uvec4 data (characterBitmap.GetValueAtIndex(j), characterBitmap.GetValueAtIndex(j + 1), characterBitmap.GetValueAtIndex(j + 2), characterBitmap.GetValueAtIndex(j + 3));
                        _characterUBO.SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&data));
                    }

                    if (numRemaining) {
                        // Fill last element (remaining number of indices).
                        const UniformBufferElement& element = characterBufferElements[bufferElementIndex];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());

                        glm::uvec4 data (0.0f);

                        for (int j = 0; j < numRemaining; ++j) {
                            data[j] = characterBitmap.GetValueAtIndex(numEvenDivisions * 4 + j); // Guaranteed to be less than a full step.
                        }

                        _characterUBO.SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&data));
                    }

                    characterBitmap.Clean();
                }
            }

            _characterUBO.Unbind();
            _isDirty = false;
        }
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
                std::getline(filestream, name); // Clear NL.

                // This prevents the file from having any spaces.
                if (name.empty()) {
                    break;
                }

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

                AddCharacter(character);
            }
        }
    }

    void AsciiCharacterMap::ConstructUniformBlock() {
        // Construct block layout.
        std::vector<UniformBufferElement> elementList;

        // Global data.
        elementList.emplace_back(ShaderDataType::IVEC2, "fontScale");
        elementList.emplace_back(ShaderDataType::UINT, "charactersInUse");

        // Character data.
        for (int character = 0; character < MAX_NUM_CHARACTERS; ++character) {
            std::string characterString = "character" + std::to_string(character);
            for (int bitmap = 0; bitmap < MAX_NUM_BITMAPS / 4; ++bitmap) {
                std::string bitmapString = "bitmap" + std::to_string(bitmap);

                elementList.emplace_back(ShaderDataType::UVEC4, characterString + bitmapString); // std140 rules - every element in an array has an alignment equal to a vec4.
            }
        }

        UniformBlockLayout characterBlockLayout;
        characterBlockLayout.SetBufferElements(2, MAX_NUM_BITMAPS / 4, elementList);

        UniformBlock characterBlock(1, characterBlockLayout);
        _characterUBO.SetUniformBlock(characterBlock);
    }

}
