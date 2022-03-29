
#include "common/api/shader/shader_uniform_lut.h"

namespace Sandbox {

    ShaderUniformLUT::ShaderUniformLUT() : root_(nullptr) {
    }

    ShaderUniformLUT::~ShaderUniformLUT() {
        // TODO:
    }

    void ShaderUniformLUT::Insert(const std::string& data) {
        if (data.empty()) {
            return;
        }

        int index = 0;

        if (!root_) {
            root_ = CreateNode(data[index]);
        }
        Insert(root_, data, index + 1);
    }

    void ShaderUniformLUT::Insert(const std::vector<std::string>& data) {
        for (const std::string& element : data) {
            Insert(element);
        }
    }

    void ShaderUniformLUT::Insert(const std::initializer_list<std::string>& data) {
        for (const std::string& element : data) {
            Insert(element);
        }
    }

    void ShaderUniformLUT::Remove(const std::string& data) {

    }

    bool ShaderUniformLUT::Exists(const std::string& data) const {
    }

    std::string ShaderUniformLUT::GetNearestNeighbor(const std::string& data) const {
        return std::string();
    }

    void ShaderUniformLUT::Insert(Node* node, const std::string& data, int index) {
        // 'data' is guaranteed to be valid (size > 0)
        if (index < data.size()) {
            Node* target = nullptr;

            if (data[index] < node->key) {
                // Smaller characters go into the left subtree.
                target = node->left;
            }
            else if (data[index] > node->key) {
                // Greater characters go into the right subtree.
                target = node->right;
            }
            else {
                // Current character is equal.
                target = node->middle;
            }

            if (!target) {
                target = CreateNode(data[index]);
            }
            Insert(target, data, index + 1);
        }
        else {
            node->end = true;
        }
    }

    ShaderUniformLUT::Node* ShaderUniformLUT::CreateNode(char data) const {
        return new Node(data);
    }

    ShaderUniformLUT::Node::Node(char inKey) : key(inKey),
                                               end(false),
                                               left(nullptr),
                                               middle(nullptr),
                                               right(nullptr)
                                               {
    }

    ShaderUniformLUT::Node::~Node() = default;

}