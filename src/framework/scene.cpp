
#include <framework/scene.h>
#include <framework/window.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    IScene::IScene(SceneOptions options) : camera_(new ICamera(static_cast<float>(Window::Instance().GetWidth()), static_cast<float>(Window::Instance().GetHeight()))),
                                           options_(options)
                                           {
    }

    IScene::~IScene() {
    }

    void IScene::OnInit() {
    }

    void IScene::OnUpdate(float dt) {
        ImGuiLog& log = ImGuiLog::Instance();

        // Recompile shaders.
        try {
            shaderLibrary_.RecompileAllModified();
        }
        catch (std::runtime_error& err) {
            log.LogError("Shader recompilation failed: %s", err.what());
        }

        lightingManager_.Update();
    }

    void IScene::OnPreRender() {
    }

    void IScene::OnRender() {
    }

    void IScene::OnPostRender() {
    }

    void IScene::OnImGui() {
        if (options_.useDocking) {
            // Enable window docking.
            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        }

        // Framework overview.
        if (ImGui::Begin("Overview")) {
            ImGui::Text("Render time:");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        ImGui::End();

        // ImGui log output.
        ImGuiLog& log = ImGuiLog::Instance();
        log.OnImGui();

        // Materials.
        materialLibrary_.OnImGui();

        // Model properties.
        modelManager_.OnImGui();
    }

    void IScene::OnShutdown() {
    }

    void IScene::OnWindowResize(int width, int height) {
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        camera_->SetAspectRatio(aspect);
    }

    void IScene::OnKeyboardInput(int key, int action) {
    }

    void IScene::OnMouseInput(int button, int action) {
    }

    void IScene::OnMouseScroll(float offset) {
//        // Zoom on scrolling the mouse.
//        camera_->SetFOV(glm::clamp(camera_->GetFOV() - offset, 1.0f, 100.0f));
    }

}
