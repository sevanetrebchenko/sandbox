
#ifndef SANDBOX_MODEL_MANAGER_H
#define SANDBOX_MODEL_MANAGER_H

#include <sandbox.h>

#include <framework/model.h>
#include <framework/object_loader.h>

namespace Sandbox {

    class ModelManager : public Singleton<ModelManager> {
        public:
            REGISTER_SINGLETON(ModelManager);

            void Initialize() override;
            void Shutdown() override;

            void OnImGui();

            Model* AddModelFromFile(const std::string& modelName, std::string filepath);

            [[nodiscard]] const std::vector<Model*>& GetModels() const;
            [[nodiscard]] Model* GetNamedModel(std::string modelName) const;

            void Clear();

        private:
            ModelManager();
            ~ModelManager() override;

            void DrawModelTransformImGui(Model* model) const;
            void DrawModelMaterialImGui(Model* model) const;

            std::vector<Model*> _modelList;
    };

}

#endif //SANDBOX_MODEL_MANAGER_H
