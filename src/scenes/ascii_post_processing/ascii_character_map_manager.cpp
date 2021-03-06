//
//#include <scenes/ascii_post_processing/ascii_character_map_manager.h>
//
//namespace Sandbox {
//
//    AsciiCharacterMapManager &AsciiCharacterMapManager::GetInstance() {
//        static AsciiCharacterMapManager instance;
//        return instance;
//    }
//
//    void AsciiCharacterMapManager::EnableCharacterMap(unsigned int fontSize) {
//        if (fontSize < 3 || fontSize > 14) {
//            throw std::out_of_range("Font size out of range into GetCharacterMap. Font size must be between 3 and 14 (inclusive).");
//        }
//
//        _enabledFontSize = fontSize;
//    }
//
//    void AsciiCharacterMapManager::UpdateData() {
//        const auto& characterMapIter = _characterMaps.find(_enabledFontSize);
//        characterMapIter->second.UpdateData();
//    }
//
//    void AsciiCharacterMapManager::SetCharacterMap(unsigned int fontSize, const AsciiCharacterMap &characterMap) {
//        if (fontSize != characterMap.GetFontSize()) {
//            throw std::runtime_error("Font sizes provided to SetCharacterMap does not match the font size of the character map.");
//        }
//        _characterMaps.emplace(fontSize, characterMap);
//        _enabledFontSize = fontSize;
//    }
//
//    AsciiCharacterMap &AsciiCharacterMapManager::GetCharacterMap(unsigned int fontSize) {
//        if (fontSize < 3 || fontSize > 14) {
//            throw std::out_of_range("Font size out of range into GetCharacterMap. Font size must be between 3 and 14 (inclusive).");
//        }
//
//        const auto& characterMapIter = _characterMaps.find(fontSize);
//        return characterMapIter->second;
//    }
//
//    AsciiCharacterMapManager::AsciiCharacterMapManager() : _enabledFontSize(5) {
//
//    }
//
//    AsciiCharacterMapManager::~AsciiCharacterMapManager() {
//
//    }
//
//}
