
#include "common/application/scene.h"
#include "common/api/shader/shader_library.h"
#include "common/utility/log.h"

namespace Sandbox {

    IScene::IScene() : isLocked_(false) {
    }

    IScene::~IScene() = default;

    void IScene::OnInit() {
    }

    void IScene::OnUpdate() {
        // Recompile shaders.
        try {
            ShaderLibrary::Instance().RecompileModified();
        }
        catch (std::runtime_error& err) {
            ImGuiLog::Instance().LogError("%s", err.what());
        }
    }

    void IScene::OnPreRender() {
    }

    void IScene::OnRender() {
    }

    void IScene::OnPostRender() {
    }

    void IScene::OnImGui() {
    }

    void IScene::OnShutdown() {
    }

    void IScene::OnWindowResize(int width, int height) {
    }

    void IScene::SetName(const std::string& name) {
        if (isLocked_) {
            LogWarningOnce("Function SetName called on a locked Scene instance - call has no effect. SetName should be called only in scene constructors or derived OnInit functions.");
            return;
        }

        // Setting the name is only possible if the scene is not locked.
        name_ = name;
    }

    void IScene::SetDataDirectory(const std::string& dataDirectory) {
        // Setting the name is only possible if the scene is not locked.
        if (isLocked_) {
            LogWarningOnce("Function SetDataDirectory calle on a locked Scene instance - call has no effect. SetDataDirectory should be called only in scene constructors or derived OnInit functions.");
            return;
        }

        dataDirectory_ = dataDirectory;
    }

    void IScene::Lock() {
        if (isLocked_) {
            LogWarningOnce("Function Lock called on a locked Scene instance - call has no effect.");
            return;
        }

        if (dataDirectory_.empty()) {
            // Data directory was not set, generate it from the name.
            std::string generatedName = ProcessName();
            dataDirectory_ = ConvertToNativeSeparators(GetWorkingDirectory() + "/data/scenes/" + generatedName);
        }

        CreateDirectory(dataDirectory_);

        uiLayout_ = ConvertToNativeSeparators(dataDirectory_ + "/ui_layout.ini");
        CreateFile(uiLayout_);

        shaderCache_ = ConvertToNativeSeparators(dataDirectory_ + "/shaders");
        CreateDirectory(shaderCache_);

        isLocked_ = true; // Prevent any future changes.
    }

    const std::string& IScene::GetName() const {
        return name_;
    }

    const std::string& IScene::GetUILayoutFile() const {
        return uiLayout_;
    }

    const std::string& IScene::GetDataDirectory() const {
        return dataDirectory_;
    }

    const std::string& IScene::GetShaderCacheDirectory() const {
        return shaderCache_;
    }

    std::string IScene::ProcessName() const {
        // Convert name to all lowercase, with underscores instead of spaces.
        std::vector<char> disallowedCharacters = { '/', '\\', ':', '*', '?', '"', '<', '>', '|' }; // Windows.

        std::stringstream builder;
        for (char character : name_) {
            if (character == ' ') {
                builder << '_';
            }
            else {
                bool skipCharacter = false;

                for (char c : disallowedCharacters) {
                    if (character == c) {
                        skipCharacter = true;
                        break;
                    }
                }

                if (skipCharacter) {
                    continue;
                }

                builder << static_cast<char>(std::tolower(character));
            }
        }

        return builder.str();
    }

}
