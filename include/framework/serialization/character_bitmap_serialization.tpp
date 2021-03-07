
#ifndef SANDBOX_CHARACTER_BITMAP_SERIALIZATION_TPP
#define SANDBOX_CHARACTER_BITMAP_SERIALIZATION_TPP

using json = nlohmann::json;

namespace nlohmann {

    template <>
    struct adl_serializer<Sandbox::CharacterBitmap> {
        static void to_json(json& j, const Sandbox::CharacterBitmap& bitmap) {
            j = json {
                    { "isDirty", bitmap._isDirty },
                    { "characterWidth", bitmap._characterWidth },
                    { "characterHeight", bitmap._characterHeight },
                    { "name", bitmap._name },
                    { "bitmap", bitmap._bitmap }
            };
        }

        static Sandbox::CharacterBitmap from_json(const json& j) {
            Sandbox::CharacterBitmap bitmap(j["name"].get<std::string>(), j["characterWidth"].get<unsigned>(), j["characterHeight"].get<unsigned>());
            bitmap._isDirty = j["isDirty"].get<bool>();
            bitmap._bitmap = j["bitmap"].get<std::vector<unsigned>>();
            return bitmap;
        }
    };

}

#endif //SANDBOX_CHARACTER_BITMAP_SERIALIZATION_TPP