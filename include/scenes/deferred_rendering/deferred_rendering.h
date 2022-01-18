
#ifndef SANDBOX_DEFERRED_RENDERING_H
#define SANDBOX_DEFERRED_RENDERING_H

#include <framework/scene.h>
#include <framework/buffer/fbo.h>
#include <framework/buffer/ubo.h>
#include <framework/lighting_manager.h>

namespace Sandbox {

    class SceneDeferredRendering : public IScene {
        public:
            explicit SceneDeferredRendering(SceneOptions options);
            ~SceneDeferredRendering() override;

        protected:
            void OnInit() override;

            void OnUpdate(float dt) override;

            void OnPreRender() override;
            void OnRender() override;
            void OnPostRender() override;

            void OnImGui() override;

            void OnShutdown() override;

        private:
            void InitializeShaders();
            void InitializeTextures();
            void InitializeMaterials();
            void ConfigureLights();
            void ConfigureModels();
            void ConstructFBO();

            void GeometryPass();
            void RenderOutputScene();
            void RenderDepthBuffer();

            FrameBufferObject _fbo;
    };

}

#endif //SANDBOX_DEFERRED_RENDERING_H
