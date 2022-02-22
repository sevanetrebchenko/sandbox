
#ifndef SANDBOX_DEFERRED_RENDERING_H
#define SANDBOX_DEFERRED_RENDERING_H

#include "common/application/scene.h"
#include "common/api/buffer/fbo.h"
#include "common/api/shader/shader.h"
#include "common/geometry/model_manager.h"
#include "common/lighting/lighting_manager.h"
#include "common/material/material_library.h"
#include "common/api/shader/shader_library.h"
#include "common/camera/fps_camera.h"

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

            MaterialLibrary materialLibrary_;
    };

}

#endif //SANDBOX_DEFERRED_RENDERING_H
