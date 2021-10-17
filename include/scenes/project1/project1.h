
#ifndef SANDBOX_PROJECT1_H
#define SANDBOX_PROJECT1_H

#include <framework/scene.h>
#include <framework/buffer/fbo.h>
#include <framework/buffer/ubo.h>
#include <framework/lighting_manager.h>

#include <framework/animation.h>
#include <framework/animator.h>
#include <framework/debug.h>

namespace Sandbox {

    class SceneProject1 : public Scene {
        public:
            SceneProject1(int width, int height);
            ~SceneProject1() override;

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
    };

}

#endif //SANDBOX_PROJECT1_H
