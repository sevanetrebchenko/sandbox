
#include "common/application/scene.h"
#include "common/api/shader/shader_library.h"
#include "common/utility/log.h"

namespace Sandbox {

    IScene::IScene() {
    }

    IScene::~IScene() {
    }

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

}
