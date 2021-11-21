
#ifndef SANDBOX_PROJECT3_H
#define SANDBOX_PROJECT3_H

#include <framework/scene.h>
#include <framework/buffer/fbo.h>
#include <framework/buffer/ubo.h>
#include <framework/lighting_manager.h>

#include <framework/animation.h>
#include <framework/animator.h>
#include <framework/debug.h>
#include <framework/animated_model.h>
#include <framework/path.h>

namespace Sandbox {

    class SceneProject3 : public Scene {
        public:
            SceneProject3(int width, int height);
            ~SceneProject3() override;

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

            void RenderSkeletonBones(AnimatedModel* animatedModel) const;
            void RenderSkeletonBone(Skeleton* skeleton, Animator* animator, const glm::mat4& parentTransform, const glm::vec3& origin, int root) const;

            void RenderAnimatedModelPath(Pather* pather) const;

            FrameBufferObject _fbo;
            DDRenderInterfaceCoreGL* _debugRenderer;
    };

}

#endif //SANDBOX_PROJECT3_H
