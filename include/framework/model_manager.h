
#ifndef SANDBOX_MODEL_MANAGER_H
#define SANDBOX_MODEL_MANAGER_H

#include <sandbox_pch.h>
#include <framework/model.h>
#include <framework/object_loader.h>

namespace Sandbox {

    class ModelManager {
        public:
            ModelManager();
            ~ModelManager();

            void OnImGui();
            void Update(float dt);

            Model* AddModelFromFile(std::string modelName, std::string filepath);

            [[nodiscard]] const std::vector<Model*>& GetModels() const;
            [[nodiscard]] Model* GetNamedModel(std::string modelName) const;

        private:
            void DrawModelTransformImGui(Model* model) const;
            void DrawModelMaterialImGui(Model* model) const;

            std::vector<Model*> _modelList;
    };

}

#endif //SANDBOX_MODEL_MANAGER_H
