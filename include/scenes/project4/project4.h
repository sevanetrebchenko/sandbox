
#ifndef SANDBOX_PROJECT4_H
#define SANDBOX_PROJECT4_H

#include <framework/scene.h>
#include <framework/buffer/fbo.h>
#include <framework/buffer/ubo.h>
#include <framework/lighting_manager.h>

#include <framework/rigid_body.h>
#include <framework/debug.h>

namespace Sandbox {

    class SceneProject4 : public Scene {
        public:
            SceneProject4(int width, int height);
            ~SceneProject4() override;

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

            FrameBufferObject _fbo;
            DDRenderInterfaceCoreGL* _debugRenderer;

            RigidBodyCollection rb_;
    };

}

#endif //SANDBOX_PROJECT4_H
