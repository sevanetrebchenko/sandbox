
#pragma once

namespace std {

    std::size_t hash<Sandbox::ShaderInclude>::operator()(const Sandbox::ShaderInclude& data) const {
        return std::hash<std::string>{}(data.filename);
    }

}