
#pragma once

#include "common/application/scene.h"
#include "common/api/buffer/fbo.h"
#include "common/api/shader/shader.h"
#include "common/material/material_library.h"
#include "common/api/shader/shader_library.h"
#include "common/camera/fps_camera.h"
#include "common/api/buffer/ubo.h"

#include "scenes/cs562/project1/light.h"

namespace Sandbox {

    class SceneCS562Project2 : public IScene {
        public:
            SceneCS562Project2();
            ~SceneCS562Project2() override;

        protected:
            void OnInit() override;

            void OnUpdate() override;

            void OnPreRender() override;
            void OnRender() override;
            void OnPostRender() override;

            void OnImGui() override;

            void OnShutdown() override;

            void OnWindowResize(int width, int height) override;

        private:
            void InitializeShaders();
            void InitializeMaterials();
            void ConfigureModels();
            void ConfigureLights();
            void ConstructFBO();
            void ConstructShadowMap();

            void GeometryPass();

            void ShadowPass();
            [[nodiscard]] glm::mat4 CalculateShadowMatrix();

            // Function called on project startup or whenever blur kernel radius changes.
            void InitializeBlurKernel();
            void BlurPass();

            // Lighting pass for global lights.
            void GlobalLightingPass();

            // Lighting pass for local lights.
            void LocalLightingPass();

            FrameBufferObject fbo_;
            FPSCamera camera_;
            MaterialLibrary materialLibrary_;
            DirectionalLight directionalLight_;

            FrameBufferObject shadowMap_;
            UniformBufferObject blurKernel_;
            unsigned blurKernelRadius_;
    };

}
