
#pragma once

#include "pch.h"

namespace Sandbox {

    // Ternary Search Tree.
    class ShaderUniformLUT {
        public:
            ShaderUniformLUT();
            ~ShaderUniformLUT();

            void Insert(const std::string& data);
            void Insert(const std::vector<std::string>& data);
            void Insert(const std::initializer_list<std::string>& data);

            void Remove(const std::string& data);

            [[nodiscard]] bool Exists(const std::string& data) const;

            [[nodiscard]] std::string GetNearestNeighbor(const std::string& data) const;

        private:
            struct Node {
                explicit Node(char inKey);
                ~Node();

                char key;
                bool end;

                Node* left;
                Node* middle;
                Node* right;
            };

            [[nodiscard]] Node* CreateNode(char data) const;
            void Insert(Node* root, const std::string& data, int index);

            Node* root_;
    };

}