
#ifndef SANDBOX_SKELETON_H
#define SANDBOX_SKELETON_H

#include <sandbox_pch.h>
#include <framework/bone.h>

namespace Sandbox {

    struct Skeleton {
        Skeleton() : _drawSkeleton(false) {}
        ~Skeleton() {}

        void DrawImGui() {
            for (int node : _roots) {
                DisplayNode(node);
            }
        }

        void AddBone(const Bone& bone) {
            _boneMapping[bone._name] = _bones.size();
            _bones.push_back(bone);
        }

        void AddRoot(int root) {
            _roots.push_back(root);
        }

        bool _drawSkeleton;
        std::vector<Bone> _bones;
        std::vector<int> _roots;
        std::unordered_map<std::string, unsigned> _boneMapping;

        private:
            void DisplayNode(int nodeIndex) {
                bool isLeafNode = _bones[nodeIndex]._children.empty();
                int flags = isLeafNode ? ImGuiTreeNodeFlags_Leaf : 0;

                if (ImGui::TreeNodeEx(_bones[nodeIndex]._name.c_str(), flags)) {
                    if (!isLeafNode) {
                        for (int childNode : _bones[nodeIndex]._children) {
                            DisplayNode(childNode);
                        }
                    }

                    ImGui::TreePop();
                }
            }
    };

}

#endif //SANDBOX_SKELETON_H
