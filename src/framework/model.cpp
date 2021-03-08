
#include <framework/model.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    Model::Model(std::string name) : _name(std::move(name))
                                     {
    }

    Model::Model(std::string name, const Transform &transform) : _name(std::move(name)),
                                                                 _transform(transform)
                                                                 {
    }

    Model::~Model() {
    }

    void Model::Update() {
        _transform.Clean();
    }

    const std::string &Model::GetName() const {
        return _name;
    }

    Mesh &Model::GetMesh() {
        return _mesh;
    }

    Transform &Model::GetTransform() {
        return _transform;
    }

    Material* Model::GetMaterial(Shader* shaderProgram) {
        auto materialIter = _materialMapping.find(shaderProgram);

        if (materialIter != _materialMapping.end()) {
            return materialIter->second;
        }

        return nullptr;
    }

    const std::vector<Material*>& Model::GetMaterialsList() const {
        return _materialList;
    }

    void Model::SetMesh(const Mesh& mesh) {
        _mesh = mesh;
        _mesh.Complete();
    }

    void Model::AddMaterial(Material* material) {
        Shader* shader = material->GetShader();

        if (shader) {
            _materialMapping[shader] = material;
            _materialList.emplace_back(material);
        }
        else {
            ImGuiLog::GetInstance().LogError("Failed to attach material: %s to model: %s (material does not have a valid shader program).", material->GetName().c_str(), _name.c_str());
        }
    }

}
