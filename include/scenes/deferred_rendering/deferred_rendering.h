
#ifndef SANDBOX_DEFERRED_RENDERING_H
#define SANDBOX_DEFERRED_RENDERING_H

#include <framework/scene.h>
#include <framework/buffer/fbo.h>
#include <framework/model_manager.h>
#include <framework/lighting_manager.h>
#include <framework/shader_library.h>
#include <framework/material_library.h>
#include <framework/camera/fps_camera.h>

namespace Sandbox {

    class SceneDeferredRendering : public IScene {
        public:
            SceneDeferredRendering();
            ~SceneDeferredRendering() override;

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
            void ConfigureLights();
            void ConfigureModels();
            void ConstructFBO();

            void GeometryPass();
            void RenderOutputScene();
            void RenderDepthBuffer();

            FrameBufferObject fbo_;
            FPSCamera camera_;

            ModelManager modelManager_;
            LightingManager lightingManager_;

            ShaderLibrary shaderLibrary_;
            MaterialLibrary materialLibrary_;
    };

}

#endif //SANDBOX_DEFERRED_RENDERING_H
