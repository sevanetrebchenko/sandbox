
#ifndef SANDBOX_SKELETON_H
#define SANDBOX_SKELETON_H

#include <sandbox_pch.h>
#include <framework/bone.h>

namespace Sandbox {

    struct Skeleton {
            Skeleton() : _drawSkeleton(false),
                         currentSelected(-1) {
            }

            ~Skeleton() {}

            void DrawImGui() {
                for (int node : _roots) {
                    DisplayNode(node);
                }
            }

            void AddBone(const Bone &bone) {
                _boneMapping[bone._name] = _bones.size();
                _bones.push_back(bone);
            }

            void AddRoot(int root) {
                _roots.push_back(root);
            }

            bool _drawSkeleton;
            std::vector<Bone> _bones;
            int currentSelected;
            std::vector<int> _roots;
            std::unordered_map<std::string, unsigned> _boneMapping;

        private:
            void DisplayNode(int nodeIndex) {
                Bone &bone = _bones[nodeIndex];

                bool isLeafNode = bone._children.empty();
                int flags = isLeafNode ? ImGuiTreeNodeFlags_Leaf : 0;

                if (ImGui::TreeNodeEx(bone._name.c_str(), flags)) {
                    if (!isLeafNode) {
                        for (int childNode : bone._children) {
                            DisplayNode(childNode);
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::PopStyleColor();
                    if (bone.selected) {
                        ImGui::Text("Currently Selected!");
                    }
                    else {
                        if (ImGui::Button(std::string("Select##" + std::to_string(nodeIndex)).c_str())) {
                            // Clear all other bones from being selected.
                            ClearSelected();

                            bone.selected = true;
                        }

                        if (ImGui::IsItemHovered()) {
                            bone.hovered = true;
                        }
                        else {
                            bone.hovered = false;
                        }
                    }
                ImGui::PushStyleColor(ImGuiCol_Text, 0xff999999);
            }

            void ClearSelected() {
                for (int node : _roots) {
                    ClearNode(node);
                }
            }

            void ClearNode(int nodeIndex) {
                Bone &bone = _bones[nodeIndex];
                bool isLeafNode = bone._children.empty();

                bone.selected = false; // Clear selected flag.
                for (int childNode : bone._children) {
                    ClearNode(childNode);
                }
            }
    };

}

#endif //SANDBOX_SKELETON_H
