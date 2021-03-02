
#ifndef SANDBOX_MODEL_H
#define SANDBOX_MODEL_H

#include <sandbox_pch.h>
#include <framework/transform.h>
#include <framework/mesh.h>
#include <framework/shader.h>
#include <framework/material.h>

namespace Sandbox {

    class Model {
        public:
            explicit Model(std::string name);
            Model(std::string name, const Transform& transform);
            ~Model();

            void Update(float dt);

            [[nodiscard]] const std::string& GetName() const;
            [[nodiscard]] Mesh& GetMesh();
            [[nodiscard]] Transform& GetTransform();
            [[nodiscard]] Material* GetMaterial(Shader* shaderProgram);
            [[nodiscard]] const std::vector<Material*>& GetMaterialsList() const;

            void SetMesh(const Mesh& mesh);
            void AddMaterial(Material* material);

        private:
            std::string _name;
            std::unordered_map<Shader*, Material*> _materialMapping;
            std::vector<Material*> _materialList;
            Mesh _mesh;
            Transform _transform;
    };

}

#endif //SANDBOX_MODEL_H
