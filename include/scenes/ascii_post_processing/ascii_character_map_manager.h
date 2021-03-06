//
//#ifndef SANDBOX_ASCII_CHARACTER_MAP_MANAGER_H
//#define SANDBOX_ASCII_CHARACTER_MAP_MANAGER_H
//
//#include <sandbox_pch.h>
//#include <scenes/ascii_post_processing/ascii_character_map.h>
//
//namespace Sandbox {
//
//    class AsciiCharacterMapManager {
//        public:
//            static AsciiCharacterMapManager& GetInstance();
//
//            void UpdateData();
//
//            void SetCharacterMap(unsigned fontSize, const AsciiCharacterMap& characterMap);
//            [[nodiscard]] AsciiCharacterMap& GetCharacterMap(unsigned fontSize);
//
//            // Bind the data related to this character map to the uniform block.
//            void EnableCharacterMap(unsigned fontSize);
//
//        private:
//            AsciiCharacterMapManager();
//            ~AsciiCharacterMapManager();
//
//            unsigned _enabledFontSize;
//            std::unordered_map<unsigned, AsciiCharacterMap> _characterMaps;
//    };
//
//}
//
//#endif //SANDBOX_ASCII_CHARACTER_MAP_MANAGER_H
