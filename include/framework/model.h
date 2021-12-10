
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

            virtual void Update(float dt);

            [[nodiscard]] const std::string& GetName() const;
            [[nodiscard]] const Mesh* GetMesh() const;
            [[nodiscard]] Transform& GetTransform();
            [[nodiscard]] Material* GetMaterial(const std::string& materialName);
            [[nodiscard]] const std::vector<Material*>& GetMaterialsList() const;

            void SetMesh(Mesh* mesh);
            void AddMaterial(Material* material);

        protected:
            std::string _name;
            std::vector<Material*> _materialList;
            Mesh* _mesh;
            Transform _transform;
    };

}

#endif //SANDBOX_MODEL_H
