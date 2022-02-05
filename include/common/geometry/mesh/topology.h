
#pragma once

#include "pch.h"

namespace Sandbox {

    enum class MeshTopology {
        LINES,
        TRIANGLES,
    };

    [[nodiscard]] GLuint GetRenderingPrimitive(MeshTopology topology);

}