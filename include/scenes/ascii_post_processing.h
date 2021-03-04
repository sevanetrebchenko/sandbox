
#ifndef SANDBOX_ASCII_POST_PROCESSING_H
#define SANDBOX_ASCII_POST_PROCESSING_H

#include <framework/scene.h>
#include <framework/buffer/fbo.h>
#include <framework/mesh.h>

namespace Sandbox {

    class SceneAsciiPostProcessing : public Scene {
        public:
            SceneAsciiPostProcessing(int width, int height);
            ~SceneAsciiPostProcessing() override;

        protected:
            void OnInit() override;

            void OnUpdate(float dt) override;

            void OnPreRender() override;
            void OnRender() override;
            void OnPostRender() override;

            void OnImGui() override;

            void OnShutdown() override;

            void LoadImGuiLayout() override;

        private:
            void InitializeShaders();
            void InitializeTextures();
            void InitializeMaterials();
            void ConfigureModels();
            void ConstructFBO();

            void RenderWithShader(Shader* shaderProgram);
            void RenderFSQ();

            FrameBufferObject _fbo;
            Mesh _fsq;
    };

}

#endif //SANDBOX_ASCII_POST_PROCESSING_H
