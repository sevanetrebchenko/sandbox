
#include <framework/model.h>

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

    void Model::Update(float dt) {
        // Update rigid body properties.
//        rigidBody_.SetPosition(_transform.GetPosition());
//        rigidBody_.SetOrientation(toMat4(_transform.GetRotation()));
        rigidBody_.Update(dt);

        _transform.SetPosition(rigidBody_.GetPosition());
        _transform.SetRotation(rigidBody_.GetOrientation());
        _transform.Clean();
    }

    const std::string &Model::GetName() const {
        return _name;
    }

    const Mesh* Model::GetMesh() const {
        return _mesh;
    }

    Transform &Model::GetTransform() {
        return _transform;
    }

    Material* Model::GetMaterial(const std::string &name) {
        for (Material* material : _materialList) {
            if (material->GetName() == name) {
                return material;
            }
        }

        return nullptr;
    }

    const std::vector<Material*>& Model::GetMaterialsList() const {
        return _materialList;
    }

    void Model::SetMesh(Mesh* mesh) {
        _mesh = mesh;
        _mesh->Complete();
    }

    void Model::AddMaterial(Material* material) {
        _materialList.emplace_back(material);
    }

    RigidBody &Model::GetRigidBody() {
        return rigidBody_;
    }

}
