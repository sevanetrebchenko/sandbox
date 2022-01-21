
#ifndef SANDBOX_ASCII_POST_PROCESSING_H
#define SANDBOX_ASCII_POST_PROCESSING_H

#include <common/application/scene.h>
#include <common/buffer/fbo.h>
#include <common/buffer/ubo.h>
#include <common/objects/mesh.h>
#include <scenes/ascii_post_processing/ascii_character_map.h>

namespace Sandbox {

    class SceneAsciiPostProcessing : public IScene {
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

        private:
            void InitializeShaders();
            void InitializeTextures();
            void InitializeMaterials();
            void ConfigureModels();
            void ConstructFBO();

            void ConstructAsciiMaps();

            FrameBufferObject _fbo;
            AsciiCharacterMap _characterMap;
    };

}

#endif //SANDBOX_ASCII_POST_PROCESSING_H
