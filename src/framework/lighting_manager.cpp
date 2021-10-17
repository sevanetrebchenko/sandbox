
#include <framework/lighting_manager.h>
#include <framework/ubo_manager.h>

namespace Sandbox {

    LightingManager::LightingManager() : _numActiveLights(0u),
                                         _isDirty(true)
                                         {
    }

    LightingManager::~LightingManager() {

    }

    void LightingManager::OnImGui() {
        ImGui::BeginChild("#scrollingSection", ImVec2(0, 0), false, ImGuiWindowFlags_None);

        for (int i = 0; i < _numActiveLights; ++i) {
            std::list<Light>::iterator lightIter = _lights.begin();
            std::advance(lightIter, i);
            Light& light = *lightIter;

            std::string lightID = std::to_string(i);
            std::string lightName = "Light " + lightID;
            if (ImGui::TreeNode(lightName.c_str())) {
                Transform& transform = light.GetTransform();

                // Position.
                glm::vec3 position = transform.GetPosition();
                ImGui::Text("Position:");
                if (ImGui::SliderFloat3(std::string("##position" + lightID).c_str(), (float *) (&position), -20.0f, 20.0f)) {
                    transform.SetPosition(position);
                }

                ImGui::Text("Ambient Color:");
                glm::vec3 ambient = light.GetAmbientColor();

                if (ImGui::ColorEdit3(std::string("##ambientColor" + lightID).c_str(), (float*)&ambient, ImGuiColorEditFlags_NoLabel)) {
                    light.SetAmbientColor(ambient);
                }

                ImGui::Text("Diffuse Color:");
                glm::vec3 diffuse = light.GetDiffuseColor();
                if (ImGui::ColorEdit3(std::string("##diffuseColor" + lightID).c_str(), (float*)&diffuse, ImGuiColorEditFlags_NoLabel)) {
                    light.SetDiffuseColor(diffuse);
                }

                ImGui::Text("Specular Color:");
                glm::vec3 specular = light.GetSpecularColor();
                if (ImGui::ColorEdit3(std::string("##specularColor" + lightID).c_str(), (float*)&specular, ImGuiColorEditFlags_NoLabel)) {
                    light.SetSpecularColor(specular);
                }

                ImGui::TreePop();
            }
        }

        ImGui::EndChild();
    }

    void LightingManager::Update() {
        if (_isDirty) {
            UniformBufferObject* ubo = UBOManager::GetInstance().GetUBO(0);
            assert(ubo);

            ubo->Bind();
            UniformBlock& lightUniformBlock = ubo->GetUniformBlock();
            UniformBlockLayout& lightBufferBlockLayout = lightUniformBlock.GetUniformBlockLayout();
            const std::vector<UniformBufferElement>& lightBufferElements = lightBufferBlockLayout.GetBufferElements();
            unsigned offset = lightBufferBlockLayout.GetInitialOffsetInElements();

            for (int i = 0; i < _numActiveLights; ++i) {
                std::list<Light>::iterator lightIter = _lights.begin();
                std::advance(lightIter, i);

                Light& light = *lightIter;
                unsigned bufferElementIndex = offset + lightBufferBlockLayout.GetIntermediateOffsetInElements() * i;

                if (light.IsDirty()) {
                    // Update all of light's data.
                    light.Clean(); // Update light data.

                    // isActive
                    {
                        const UniformBufferElement& element = lightBufferElements[bufferElementIndex];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
                        bool isActive = light.IsActive();
                        ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&isActive));
                    }

                    ++bufferElementIndex;

                    // position
                    {
                        const UniformBufferElement& element = lightBufferElements[bufferElementIndex];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
                        const glm::vec3& position = light.GetTransform().GetPosition();
                        ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&position));
                    }

                    ++bufferElementIndex;

                    // ambientColor
                    {
                        const UniformBufferElement& element = lightBufferElements[bufferElementIndex];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
                        const glm::vec3& ambientColor = light.GetAmbientColor();
                        ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&ambientColor));
                    }

                    ++bufferElementIndex;

                    // diffuseColor
                    {
                        const UniformBufferElement& element = lightBufferElements[bufferElementIndex];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
                        const glm::vec3& diffuseColor = light.GetDiffuseColor();
                        ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&diffuseColor));
                    }

                    ++bufferElementIndex;

                    // specularColor
                    {
                        const UniformBufferElement& element = lightBufferElements[bufferElementIndex];
                        unsigned elementOffset = element.GetBufferOffset();
                        unsigned elementDataSize = UniformBufferElement::UBOShaderDataTypeSize(element.GetShaderDataType());
                        const glm::vec3& specularColor = light.GetSpecularColor();
                        ubo->SetSubData(elementOffset, elementDataSize, static_cast<const void*>(&specularColor));
                    }
                }
            }

            _isDirty = false;
            ubo->Unbind();
        }
    }

    void LightingManager::AddLight(const Light& light) {
        _lights.push_back(light);
        ++_numActiveLights;
        _isDirty = true;
    }

}
