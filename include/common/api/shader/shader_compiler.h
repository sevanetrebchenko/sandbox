
#pragma once

#include "pch.h"

namespace Sandbox {

    // Compile
    // Note: function assumes 32-bit integer.
    [[nodiscard]] std::vector<unsigned> CompileToSPIRV(const ShaderInfo& shader);

}