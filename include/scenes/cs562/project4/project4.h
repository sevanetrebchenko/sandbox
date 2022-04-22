
#pragma once

#include "common/application/scene.h"
#include "common/api/buffer/fbo.h"
#include "common/api/shader/shader.h"
#include "common/material/material_library.h"
#include "common/api/shader/shader_library.h"
#include "common/camera/fps_camera.h"
#include "common/api/buffer/ubo.h"
#include "common/geometry/bounds.h"
#include "scenes/cs562/project1/light.h"

namespace Sandbox {

    class SceneCS562Project4 : public IScene {
        public:
            SceneCS562Project4();
            ~SceneCS562Project4() override;

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
            void InitializeTextures();
            void ConfigureModels();
            void ConfigureLights();
            void ConstructFBO();
            void ConstructShadowMap();
            void ConstructAOFBO();

            void GeometryPass();

            [[nodiscard]] glm::mat4 CalculateShadowMatrix();

            // Function called on project startup or whenever blur kernel radius changes.
            void InitializeBlurKernel();

            // Lighting pass for global lights.
            void GlobalLightingPass();

            // Lighting pass for local lights.
            void LocalLightingPass();

            void GenerateShadowMap();
            void BlurShadowMap();

            void GenerateRandomPoints();

            void GenerateIrradianceMap(std::string filename) const;
            void RenderSkydome();

            void AOPass();
            void BlurAO();

            Bounds bounds_;

            FrameBufferObject fbo_;
            FPSCamera camera_;
            MaterialLibrary materialLibrary_;
            DirectionalLight directionalLight_;

            FrameBufferObject shadowMap_;
            UniformBufferObject blurKernel_;
            int blurKernelRadius_;

            int brdfModel_;

            UniformBufferObject randomPoints_;
            Texture environmentMap_;
            Texture irradianceMap_;
            float exposure_;
            float contrast_;

            FrameBufferObject ao_;
            float ambientOcclusionRadius_;
            float ambientOcclusionScale_;
            float ambientOcclusionContrast_;
    };

}
