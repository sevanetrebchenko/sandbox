
#ifndef SANDBOX_ANIMATION_H
#define SANDBOX_ANIMATION_H

#include <sandbox_pch.h>
#include <framework/model.h>
#include <framework/bone.h>

namespace Sandbox {

    struct AnimationNode {
        glm::mat4 _transform;
        std::string _name;
        std::vector<AnimationNode> _children;
    };

    class Animation {
        public:
            Animation(const std::string& filepath, Model* model);
            ~Animation();

            [[nodiscard]] Bone* GetBone(const std::string& boneName);
            [[nodiscard]] float GetSpeed() const;
            [[nodiscard]] float GetDuration() const;
            [[nodiscard]] const AnimationNode& GetRootNode() const;
            [[nodiscard]] const Model* GetBoundModel() const; // Get the model this animation is tied to.

        private:
            void ProcessAnimationNode(const aiNode* root, AnimationNode& data);

            Model* _model;
            float _speed;
            float _duration;
            std::vector<Bone> _bones;
            AnimationNode _root;
    };

}

#endif //SANDBOX_ANIMATION_H
